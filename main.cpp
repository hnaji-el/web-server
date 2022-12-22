// #pragma once

#include "request_data.hpp"
#include "utils.hpp"
#include <cstdlib>
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
#define BUFF_SIZE 1024
#define BIGFILE_SIZE 104857603
#define MEDIUMFILE_SIZE 52428803
#define SMALLFILE_SIZE 200

int main() {
	// getting servers from config file
	std::vector<server> s;
	s.push_back(server(3333));
	s.push_back(server(5555));

	// servers sockets to add to set
	std::vector<int> servers_socks;
	for (std::vector<server>::iterator it = s.begin(); it != s.end(); it++)
		servers_socks.push_back((*it).sock_fd);

	// buffer for request read/writing and reponse file reading
	char buff[BUFF_SIZE + 1];
	bzero(buff, BUFF_SIZE + 1);

	// fd_set current_sockets, ready_sockets;
	fd_set current_sockets;
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
		unsigned long long f = 0;
	while (1) 
	{
		// ready_sockets = current_sockets;
		// write_set = current_sockets;
		// read_set = current_sockets;
		write_set = read_set = current_sockets;
		select(FD_SETSIZE, &read_set, &write_set, NULL, NULL);
		for (int i = 0; i <= max_fd; i++) {
			if (FD_ISSET(i, &read_set))
			{
				if (std::find(servers_socks.begin(), servers_socks.end(), i) != servers_socks.end())
				{
					int conn_fd = accept(i, NULL, NULL);
					max_fd = (conn_fd > max_fd) ? conn_fd : max_fd;
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
							request[i].req_file.open("/tmp/" + random_string());
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
						if (read_n == 0)
						{
							printf("READ_N == 0\n");
							exit(1);
						}
						request[i].req_file.write(buff, read_n);
						request[i].req_file.flush();
						if (request[i].req_file.bad())
						{
							printf("BAD WRITE\n");
							exit(1);
						}
						usleep(100);
						f++;
						if (read_n < BUFF_SIZE)
						{
							printf("%llu LOOPS\n", f);
							f = 0;
							request[i].finished = true;
							request[i].started = false;
							request[i].req_file.close();
							request[i].req_file.clear();
						}
					}
				}
			}
			if (FD_ISSET(i, &write_set) && request[i].finished)
			{

					if (!request[i].res_started)
					{
						char hello[100] = "HTTP/1.1 200 OK\nContent-Type: "
								"text/plain\nContent-Length: 52428800\n\n";
						send(i, hello, strlen(hello), 0);
						request[i].res_file_fd = open("mediumfile",  O_RDONLY);
						request[i].res_started = true;
					}
					bzero(buff, BUFF_SIZE);
					if((read_n = read(request[i].res_file_fd, buff, BUFF_SIZE)) < 0)
						printf("READ ERROR\n");
					if(send(i, buff, read_n, 0) < 0)
						printf("WRITE ERROR\n");
					if (read_n < BUFF_SIZE)
					{
						request[i].finished = false;
						request[i].res_started = false;						
						request[i].res_file_fd = -1;
						close(request[i].res_file_fd );
					}
			}
		}
	}

	return 0;
}