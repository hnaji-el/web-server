#include <netinet/in.h>
#include <netinet/ip.h>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
class server {
	public:
		int port;
		int sock_fd;
	public:
		server(int port);
};
