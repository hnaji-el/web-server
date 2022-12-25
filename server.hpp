#include "parseConfigFile/ServerData.hpp"
#include <netinet/in.h>
#include <netinet/ip.h>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include "parseConfigFile/ServerData.hpp"
#include <cstdio>
#include <cstring>
#include <sys/errno.h>
class server
{
	public:
		int sock_fd;
	public:
		server(const ServerData &Data);
};