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
#include <vector>
#define PORT 3333
#define BUFF_SIZE 10000
// #define PORT 222
#define SA struct sockaddr

class request {
public:
	int fd;
	std::string file_name;
	bool finished;

public:
	request(int fd)
			: fd(fd), file_name("file" + std::to_string(fd)), finished(false) {}
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
	FD_ZERO(&current_sockets);
	FD_SET(s.sock_fd, &current_sockets);
	std::vector<request> v_req;
	while (1) {
		ready_sockets = current_sockets;
		select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL);
		for (int i = 0; i < FD_SETSIZE; i++) {

			if (FD_ISSET(i, &ready_sockets))
			{
				if (i == s.sock_fd)
				{
					printf("HELLO\n");
					int conn_fd = accept(s.sock_fd, (SA *)&cli_addr, &cli_addr_len);
					v_req.push_back(request(conn_fd));
					FD_SET(conn_fd, &current_sockets);
				}
				else {
					bzero(buff, BUFF_SIZE);
					// printf("CONNECTION ESTABLISHED\n");
					if (read(i, buff, BUFF_SIZE) < 0) {
						printf("ERROR READ");
						exit(1);
					}
					char hello[100] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
														
					if (std::string(buff).find("lll") != std::string::npos)
					{
						
						write(i, hello, strlen(hello));
					}
				}
			}
		}
		// printf("%s\n",buff);
		// write(file_fd, buff, BUFF_SIZE);
	}

	return 0;
}