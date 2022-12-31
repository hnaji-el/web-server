
#include "./parseConfigFile/parseConfigFile.hpp"
#include "./parseRequest/Request.hpp"
#include "./handleResponse/response.hpp"
#include "./socket/socket.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/_types/_fd_def.h>
#include <sys/select.h>
#include <algorithm>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h> 
#include <sys/types.h>

#define TIME_LIMIT_SEC 100

size_t  stringToNumber(const std::string& str)
{
	size_t  num = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		num *= 10;
		num = num + (str[i] - '0');
	}
	return (num);
}

ServerData&	matchServerBlock(std::vector<ServerData>& ser, Request& req)
{
	size_t			pos = 0;
	unsigned short	port = 0;
	size_t			index = 0;

	if (!req.headers.count("Host")) // Bad request 400
		return (ser[0]);
	pos = req.headers["Host"].find(":");
	if (pos == std::string::npos)
		port = 80;
	else
		port = stringToNumber(req.headers["Host"].substr(pos + 1, std::string::npos));

	for (; index < ser.size(); index++)
	{
		if (port == ser[index].port)
			break ;
	}
	return (ser[index]);
}

bool isTimePassed(time_t last_time_used)
{
    if (time(NULL) - last_time_used >= TIME_LIMIT_SEC)
        return true;
    return false;
}

int main(int argc, char** argv)
{
	std::vector<ServerData>	cData;
	std::vector<int> 		serversSockets;
	fd_set 					currentSocketsSet,writeSet,readSet;
	RequestMap				request;
	char 					buff[BUFF_SIZE + 1];
	int 					read_n = 0;

	FD_ZERO(&currentSocketsSet);
	if (parseConfigFile(argc, argv, cData) == -1)
		return (EXIT_FAILURE);
	for (size_t i = 0; i < cData.size(); i++)
		serversSockets.push_back(createServerSocket(cData[i]));
	for (size_t i = 0; i < serversSockets.size(); i++)
	{
		FD_SET(serversSockets[i], &currentSocketsSet);
		printf("socket = %d\n", serversSockets[i]);
	}
	printf("size = %lu\n", serversSockets.size());
	int max_fd = *std::max_element(serversSockets.begin(), serversSockets.end());
	RequestMap::iterator tmp;
	while (1) 
	{
		for (RequestMap::iterator it = request.begin(), tmp = it;  it != request.end(); it = tmp)
		{
			tmp++;
			if (it->second.fd != -1 && isTimePassed(it->second.lastTimeUsed))
			{
				closeConnection(it->first, request, &currentSocketsSet);	
			}
		}
		writeSet = readSet = currentSocketsSet;
		select(FD_SETSIZE, &readSet, &writeSet, NULL, NULL);
		for (int i = 0; i <= max_fd; i++)
		{
			if (FD_ISSET(i, &readSet) && request[i].state == STARTED)
			{
				if (std::find(serversSockets.begin(), serversSockets.end(), i) != serversSockets.end())
					acceptConnection(request, max_fd, &currentSocketsSet, i);
				else
				{
					request[i].lastTimeUsed = time(NULL); // if request  still going
					bzero(buff, BUFF_SIZE + 1);
					if ((read_n = read(i, buff, BUFF_SIZE)) == 0)
					{
						std::cout << "read == 0 " << std::endl;
						closeConnection(i, request, &currentSocketsSet);
						continue;
					}
					request[i](std::string(buff, read_n));
				}
			}
			if (FD_ISSET(i, &writeSet) && request[i].state == FINISHED)
			{
				request[i].lastTimeUsed = time(NULL); // if response  still going
				if (request[i].resFlag == HEADERNOTSENT)
				{
					handle_response(matchServerBlock(cData, request[i]), request[i]);
				}
				if (request[i].resState == BODYNOTSENT)
					sendingResponse(request[i]);
				if (request[i].resState == BODYSENT)
				{
					close(request[i].fdBody); // fdBody initialized
					if (request[i].headers.count("Connection") == 0 || request[i].headers["Connection"] != "keep-alive")
					{
						closeConnection(i, request, &currentSocketsSet);
					}
					else
						request[i].clear();
				}
			}
		}
	}
	return 0;
}