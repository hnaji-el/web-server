// #pragma once
#include "./request_data.hpp"
#include "./utils.hpp"
#include "./server.hpp"
#include "./parseConfigFile/parseConfigFile.hpp"

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
	char buff[BUFF_SIZE];
	bzero(buff, BUFF_SIZE);
	int read_n = 0;
	fd_set current_sockets,write_set, read_set;
	FD_ZERO(&current_sockets);
	for (size_t i = 0; i < servers_socks.size(); i++)
	{
		FD_SET(servers_socks[i], &current_sockets);
		printf("socket = %d\n", servers_socks[i]);
	}
	printf("size = %lu\n", servers_socks.size());

	std::map<int, request_data> request; // ------------->

	int max_fd = *std::max_element(servers_socks.begin(), servers_socks.end());
	while (1) 
	{
		write_set = read_set = current_sockets;
		select(FD_SETSIZE, &read_set, &write_set, NULL, NULL);
		for (int i = 0; i <= max_fd; i++)
		{
			// printf("X\n");
			if (FD_ISSET(i, &read_set) && !request[i].res_started)
			{
				if (std::find(servers_socks.begin(), servers_socks.end(), i) != servers_socks.end())
				{
					printf("CONNECTION\n");
					int conn_fd = accept(i, NULL, NULL);
					if (fcntl(conn_fd, F_SETFL, O_NONBLOCK) < 0)
					{
						printf("FCNTL FAILURE\n");
						exit(1);
					}
					max_fd = (conn_fd > max_fd) ? conn_fd : max_fd;
					request[conn_fd] = request_data(conn_fd);
					FD_SET(conn_fd, &current_sockets);
				}
				else
				{
					bzero(buff, BUFF_SIZE);
					if (!request[i].finished)
					{
						// parsing(request[i], buff); ========>
						if (!request[i].started)
						{
							// usleep(1000000);
							request[i].started = true;
							request[i].req_file_fd = open(("/tmp/" + random_string()).c_str(), O_CREAT | O_WRONLY , 0666);
							if (request[i].req_file_fd < 0)
							{
								// std::cout << "REQ FILE FAIL" << std::endl;
								printf("REQ FILE FAIL\n");
								exit(1);
							}
							printf("A\n");
						}
						usleep(50);
						if ((read_n = read(i, buff, BUFF_SIZE)) <= 0) {
							printf("READ <= 0\n");
							close(i);
							FD_CLR(i, &current_sockets);
							continue;
						}
						write(request[i].req_file_fd, buff, read_n);
						usleep(100);
						request[i].loop++;
						if (read_n < BUFF_SIZE)
						{
							printf("%llu LOOPS\n", request[i].loop);
							printf("%d\n", read_n);
							request[i].loop = 0;
							//parsing 

							request[i].finished = true;
							close(request[i].req_file_fd);
						}
					}
				}
			}
			if (FD_ISSET(i, &write_set) && request[i].finished)
			{



					// handle_response(match_server(request[i]), );
					// printf("RESPONSE\n");
					if (!request[i].res_started)
					{


						char hello[100] = "HTTP/1.1 200 OK\nContent-Type: "
								"text/plain\nContent-Length: 52428803\n\n";
						if (send(i, hello, strlen(hello), 0) == -1)
						{
							printf("SEND FAILURE\n");
							exit(EXIT_FAILURE);
						}
						request[i].res_file_fd = open("mediumfile",  O_RDONLY);
						request[i].res_started = true;
						// printf("HEADER\n");
					}

					bzero(buff, BUFF_SIZE);
					if((read_n = read(request[i].res_file_fd, buff, BUFF_SIZE)) < 0)
						printf("READ ERROR\n");

					if(send(i, buff, read_n, 0) < -1)
					{
						printf("%d\n",read_n);
						printf("WRITE ERROR\n");
					}
					// printf("read_n = %d\n", read_n);
					if (read_n < BUFF_SIZE)
					{
						printf("RESPONSE\n");
						request[i].started = false;
						request[i].finished = false;
						request[i].res_started = false;						
						request[i].res_file_fd = -1;
						close(request[i].res_file_fd );
					}
					// printf("EXIT INSIDE\n");

			}
			// printf("EXIT OUTSIDE\n");
			// exit(1);
		}
	}

	return 0;
}