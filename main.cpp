#include <cstdio>
#include <exception>
#include <string>
#include <strings.h>
#include <sys/select.h>
// #include <sys/_endian.h>
#include <sys/_select.h>
#include <sys/_types/_fd_def.h>
#include <sys/socket.h>
// #include <sys/_types/_socklen_t.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <unistd.h> // read(), write(), close()
#include <utility>
#include <vector>
#include <map>
#define PORT 3333
#define BUFF_SIZE 10
// #define PORT 222
#define SA struct sockaddr

class request_data {
public:
	std::string file_name;
	bool finished;

public:
	request_data(){}
	request_data(int fd)
			: file_name("file" + std::to_string(fd)), finished(false) {}
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
			if (this->sock_fd == -1) {
				printf("socket creation failed...\n");
				exit(0);
			} else
				printf("Socket successfully created..\n");
			bzero(&serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			serv_addr.sin_port = htons(this->port);

			if ((bind(this->sock_fd, (SA *)&serv_addr, sizeof(serv_addr))) != 0) {
				std::cout << "FAILED BIND" << std::endl;
				exit(1);
			}
			if ((listen(this->sock_fd, 5)) != 0) {
				std::cout << "FAILED LISTEN" << std::endl;
				exit(1);
			}
		}

};
int main() {
	struct sockaddr_in cli_addr;
	socklen_t cli_addr_len;

	server s(3333);
	char buff[BUFF_SIZE];
	cli_addr_len = sizeof(cli_addr);

	fd_set current_sockets, ready_sockets;
	fd_set write_set, read_set;
	FD_ZERO(&current_sockets);
	FD_SET(s.sock_fd, &current_sockets);
	std::map<int, request_data> request;
	int fd_input = -1;
	while (1) {
		ready_sockets = current_sockets;
		write_set = current_sockets;
		read_set = current_sockets;
		select(FD_SETSIZE, &read_set, &write_set, NULL, NULL);

		for (int i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &read_set))
			{
				if (i == s.sock_fd)
				{
					printf("HELLO\n");
					int conn_fd = accept(s.sock_fd, (SA *)&cli_addr, &cli_addr_len);
					request[conn_fd] = request_data(conn_fd);
					FD_SET(conn_fd, &current_sockets);
				}
				else {
					bzero(buff, BUFF_SIZE);
					if (!request[i].finished)
					{
						if (read(i, buff, BUFF_SIZE) < 0) {
							printf("ERROR READ");
							exit(1);
						}
						if(std::string(buff).find("lll") != std::string::npos)
							request[i].finished = true;
					}
				}
			}
			if (request[i].finished)
				printf("A\n");
			if (FD_ISSET(i, &write_set) && request[i].finished)
			{
					char hello[100] = "HTTP/1.1 200 OK\nContent-Type: "
								"text/plain\nContent-Length: 200\n\n";
														// char write_buff[BUFF_SIZE];								
					if (fd_input == -1)
					{
						write(i, hello, strlen(hello));
						fd_input = open("smallfile",  O_RDONLY);
					}

					char write_buff[21];
					bzero(write_buff, 21);
					if(read(fd_input, write_buff, 20) < 0)
						printf("READ ERROR\n");
					if(write(i, write_buff, 20) < 0)
						printf("WRITE ERROR\n");					
					if(std::string(write_buff).find("lll") != std::string::npos)
					{
						request[i].finished = false;
						fd_input = -1;
					}
			}
		}
	}

	return 0;
}