// #pragma once

#include "request_data.hpp"
#include "utils.hpp"
#include <fstream>
#include <cstdio>
#include <arpa/inet.h>
#include <exception>
#include <iterator>
#include <string>
#include <strings.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <cerrno>
#include <sys/_select.h>
#include <algorithm>
#include <sys/socket.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <unistd.h> // read(), write(), close()
#include <utility>
#include <vector>
#include <map>
#include "server.hpp"
#define PORT 3333
#define BUFF_SIZE 1000000
#define BIGFILE_SIZE 104857603
#define MEDIUMFILE_SIZE 52428803
#define SMALLFILE_SIZE 200

int main() {
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len;

	std::vector<server> s;
	s.push_back(server(3333));
	s.push_back(server(5555));
	std::vector<int> servers_socks;
	for (std::vector<server>::iterator it = s.begin(); it != s.end(); it++)
		servers_socks.push_back((*it).sock_fd);
	char buff[BUFF_SIZE];
	cli_addr_len = sizeof(cli_addr); // ~NULL

	fd_set current_sockets, ready_sockets;
	fd_set write_set, read_set;
	FD_ZERO(&current_sockets);
	for (std::vector<int>::iterator it = servers_socks.begin(); it != servers_socks.end(); it++)
	{
		printf("A\n");
		FD_SET(*it, &current_sockets);
	}
	std::map<int, request_data> request;
	int read_n = 0;
	int max_fd = *std::max_element(servers_socks.begin(), servers_socks.end());
	// printf("max_Fd: %d", max_fd);
	// exit(1);
	while (1) 
	{
		ready_sockets = current_sockets;
		write_set = current_sockets;
		read_set = current_sockets;
		select(FD_SETSIZE, &read_set, &write_set, NULL, NULL);
		for (int i = 0; i <= max_fd; i++) {
			if (FD_ISSET(i, &read_set))
			{
				if (std::find(servers_socks.begin(), servers_socks.end(), i) != servers_socks.end())
				{
					printf("NEW CONNECTION GETTING ACCEPTED\n");
					int conn_fd = accept(i, (struct sockaddr *)&cli_addr, &cli_addr_len);
					max_fd = (conn_fd > max_fd) ? conn_fd : max_fd;
					printf("max_fd: %d", max_fd);
					request[conn_fd] = request_data(conn_fd);
					FD_SET(conn_fd, &current_sockets);
				}
				else 
				{
					bzero(buff, BUFF_SIZE);
					if (!request[i].finished)
					{
						if (!request[i].started)
						{
							request[i].started = true;
							request[i].req_file.open("/tmp/" + random_string(), std::ofstream::out | std::ofstream::app);
							if (request[i].req_file.fail())
							{
								std::cout << "REQ FILE FAIL\n";
								exit(1);
							}
						}
						if ((read_n = recv(i, buff, BUFF_SIZE, 0)) < 0) {
							printf("ERROR READ");
							exit(1);
						}
						
						request[i].req_file.write(buff, read_n);
						request[i].req_file.flush();
						if (request[i].req_file.bad())
						{
							printf("BAD WRITE\n");
							exit(1);
						}
							// request[i].req_file << "HELLO";
							// printf("LOOP\n");
						// if(write(request[i].req_file_fd, buff, read_n) < 0)
						// {
						// 	printf("WRITE ERROR\n");
						// 	exit(1);
						// }
						// request[i].req_file << buff;
						if (read_n < BUFF_SIZE)
						{
							request[i].finished = true;
							//parsing
							// sleep(10);
							printf("AFTER SLEEP\n");
							// request[i].req_file_fd = open(request[i].req_file_name.c_str(), O_RDONLY | O_WRONLY | O_TRUNC);
							// clear_file(request[i].req_file_name);
						}
					}
				}
			}
			if (FD_ISSET(i, &write_set) && request[i].finished)
			{
					if (!request[i].res_started)
					{
						char hello[100] = "HTTP/1.1 200 OK\nContent-Type: "
								"text/plain\nContent-Length: 52428803\n\n";
						send(i, hello, strlen(hello), 0);
						request[i].res_file_fd = open("mediumfile",  O_RDONLY);
						printf("%d", request[i].res_file_fd);
						request[i].res_started = true;
					}
					char write_buff[BUFF_SIZE];
					bzero(write_buff, BUFF_SIZE);
					if((read_n = read(request[i].res_file_fd, write_buff, BUFF_SIZE)) < 0)
						printf("READ ERROR\n");
					if(send(i, write_buff, read_n, 0) < 0)
						printf("WRITE ERROR\n");
					if (read_n < BUFF_SIZE)
					{
						printf("FINISHED\n");
						printf("read_n %d-----%d\n",i, read_n);
						request[i].finished = false;
						request[i].res_file_fd = -1;
						close(request[i].res_file_fd );
						// break;
						// exit(1);
					}
			}
		}
	}

	return 0;
}