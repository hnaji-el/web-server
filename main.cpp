
#include "./utils.hpp"
#include "./server.hpp"
#include "./parseConfigFile/parseConfigFile.hpp"
#include "./parseRequest/Request.hpp"
#include "./handleResponse/response.hpp"

#include <cstdio>
#include <string>
#include <sys/select.h>
#include <algorithm>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h> 
#include <sys/types.h>

#define BUFF_SIZE 1024

int main(int argc, char** argv)
{
	std::vector<ServerData>	cData;
	std::map<int, Request>	request;

	if (parseConfigFile(argc, argv, cData) == -1)
		return (EXIT_FAILURE);

	// getting servers from config file
	std::vector<server> s;
	for (size_t i = 0; i < cData.size(); i++)
		s.push_back(server(cData[i]));

	// servers sockets to add to set
	std::vector<int> servers_socks;
	for (size_t i = 0; i < s.size(); i++)
		servers_socks.push_back(s[i].sock_fd);

	// buffer for request read/writing and reponse file reading
	char buff[BUFF_SIZE + 1];
	int read_n = 0;
	fd_set current_sockets,write_set, read_set;
	FD_ZERO(&current_sockets);
	for (size_t i = 0; i < servers_socks.size(); i++)
	{
		FD_SET(servers_socks[i], &current_sockets);
		printf("socket = %d\n", servers_socks[i]);
	}
	printf("size = %lu\n", servers_socks.size());


	int max_fd = *std::max_element(servers_socks.begin(), servers_socks.end());
	while (1) 
	{
		write_set = read_set = current_sockets;
		select(FD_SETSIZE, &read_set, &write_set, NULL, NULL);
		for (int i = 0; i <= max_fd; i++)
		{
			if (FD_ISSET(i, &read_set) && request[i].state == STARTED)
			{
				if (std::find(servers_socks.begin(), servers_socks.end(), i) != servers_socks.end())
				{
					std::cout << "CONNECTION" << std::endl;
					const int	conn_fd = accept(i, NULL, NULL);
					if (fcntl(conn_fd, F_SETFL, O_NONBLOCK) < 0)
					{
						std::cout << "FCNTL FAILURE" << std::endl;
						exit(1);
					}
					max_fd = (conn_fd > max_fd) ? conn_fd : max_fd;
					request[conn_fd] = Request(conn_fd);
					FD_SET(conn_fd, &current_sockets);
				}
				else
				{
					bzero(buff, BUFF_SIZE);
					if ((read_n = read(i, buff, BUFF_SIZE)) <= 0)
					{
						printf("READ <= 0\n");
						close(i);
						request.erase(i);
						FD_CLR(i, &current_sockets);
						continue ;
					}
					usleep(100);
					request[i](buff);
				}
			}
			if (FD_ISSET(i, &write_set) && request[i].state == FINISHED)
			{
				// Need to match server ...
				if (request[i].resFlag == HEADERNOTSENT)
					handle_response(cData[0], request[i]);
				if (request[i].resState == BODYNOTSENT)
				{
					char buff[10001];
					bzero(&buff, 10001);
					int r = read(request[i].fdBody, buff, 10000);
					write(i, buff, r);
					// reading from the bodyPath and write ...
				}
				if (request[i].resState == BODYSENT)
				{
					request[i].clear();
					close(request[i].fdBody);
				}
			}
		}
	}

	return 0;
}