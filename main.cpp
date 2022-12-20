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
// #include <sys/_endian.h>
#include <cerrno>
#include <sys/_select.h>
#include <algorithm>
// #include <sys/_types/_fd_def.h>
#include <sys/socket.h>
// #include <sys/_types/_socklen_t.h>
#include <fcntl.h>
#include <iostream>
// #include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <unistd.h> // read(), write(), close()
#include <utility>
#include <vector>
#include <map>
#define PORT 3333
#define BUFF_SIZE 300
#define BIGFILE_SIZE 104857603
#define MEDIUMFILE_SIZE 52428803
#define SMALLFILE_SIZE 200
// #define PORT 222
std::string RandomString()
{
	srand(time(NULL));
    char alpha[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                          'h', 'i', 'j', 'k', 'l', 'm', 'n',
                          'o', 'p', 'q', 'r', 's', 't', 'u',
                          'v', 'w', 'x', 'y', 'z' };
    std::string result = "";
    for (int i = 0; i<10; i++)
        result = result + alpha[rand() % 26];
    return result;
}

class request_data {
public:
	// std::string req_file;
	std::ofstream req_file;
	// int req_file_fd;
	bool started;
	bool finished;
	int fd;
	int res_file_fd;
	std::string req_file_name;


public:
	request_data(){}
	request_data(int fd)
			: finished(false),fd(fd), res_file_fd(-1) 
	{

		// std::cout << "filename:" << this->req_file << std::endl;
		// this->req_file.open(this->req_file.c_str(),  O_CREAT | O_WRONLY);

		// if (this->req_file < 0){

       	// 	std::cout << "OPEN: " << std::strerror(errno) << '\n';
		// 	exit(1);
		// }
	}
	request_data &operator=(const request_data& a)
	{
		// (void)a;
		std::cout << "OPERATOR\n";
		this->fd = a.fd;
		this->finished = a.finished;
		// this->req_file = a.req_file;
		this->res_file_fd = a.res_file_fd;
		return *this;
	}
};

class server {
	public:
		int port;
		int sock_fd;
	public:
		server(int port) :port(port)
		{
			struct sockaddr_in serv_addr;
			this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
			int a = 1;
			if (setsockopt(this->sock_fd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int)) < 0)
    			printf("setsockopt(SO_REUSEADDR) failed\n");
			if (this->sock_fd == -1) {
				printf("socket creation failed...\n");
				exit(0);
			} else
				printf("Socket successfully created..\n");
			bzero(&serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
			serv_addr.sin_port = htons(this->port);

			if ((bind(this->sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) != 0) {
       			 std::cout << "BIND: " << std::strerror(errno) << '\n';
				exit(1);
			}
			if ((listen(this->sock_fd, 5)) != 0) {
				std::cout << "FAILED LISTEN" << std::endl;
				exit(1);
			}
		}

};

int main() {
	// std::cout << std::tmpnam(NULL) << std::endl;
	// exit(1);
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len;

	std::vector<server> s;
	// 
	s.push_back(server(3333));
	s.push_back(server(5555));
	std::vector<int> servers_socks;
	for (std::vector<server>::iterator it = s.begin(); it != s.end(); it++)
		servers_socks.push_back((*it).sock_fd);
	char buff[BUFF_SIZE];
	cli_addr_len = sizeof(cli_addr);

	fd_set current_sockets, ready_sockets;
	fd_set write_set, read_set;
	FD_ZERO(&current_sockets);
	for (std::vector<int>::iterator it = servers_socks.begin(); it != servers_socks.end(); it++)
	{
		printf("A\n");
		FD_SET(*it, &current_sockets);
	}
	std::map<int, request_data> request;
	// request_data a(1);
	// request[1] = a;
	// (void)a;
	int read_n = 0;
	while (1) 
	{
		ready_sockets = current_sockets;
		write_set = current_sockets;
		read_set = current_sockets;
		select(FD_SETSIZE, &read_set, &write_set, NULL, NULL);
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &read_set))
			{
				if (std::find(servers_socks.begin(), servers_socks.end(), i) != servers_socks.end())
				{
					printf("NEW CONNECTION GETTING ACCEPTED\n");
					int conn_fd = accept(i, (struct sockaddr *)&cli_addr, &cli_addr_len);
					request[conn_fd] = request_data(conn_fd);
					request[conn_fd].req_file.open("/tmp/" + RandomString(), std::ofstream::out | std::ofstream::app);
					if (request[conn_fd].req_file.fail())
					{
						std::cout << "REQ FILE FAIL\n";
						exit(1);
					}
					FD_SET(conn_fd, &current_sockets);
				}
				else 
				{
					bzero(buff, BUFF_SIZE);
					if (!request[i].finished)
					{
						if ((read_n = recv(i, buff, BUFF_SIZE, 0)) < 0) {
							printf("ERROR READ");
							exit(1);
						}
						for(int i = 0; i < read_n; i++)
						{
							// request[i].req_file << buff[i];
							request[i].req_file << "HELLO";
							// printf("LOOP\n");
						}
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
							clear_file(request[i]);
						}
					}
				}
			}
			if (FD_ISSET(i, &write_set) && request[i].finished)
			{

					char hello[100] = "HTTP/1.1 200 OK\nContent-Type: "
								"text/plain\nContent-Length: 52428803\n\n";
					if (request[i].res_file_fd == -1)
					{
						send(i, hello, strlen(hello), 0);
						request[i].res_file_fd = open("mediumfile",  O_RDONLY);
						printf("%d", request[i].res_file_fd);
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
						request[i].finished = false;
						request[i].res_file_fd = -1;
						close(request[i].res_file_fd );
					}
			}
		}
	}

	return 0;
}