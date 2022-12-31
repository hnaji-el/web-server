#include "../parseConfigFile/ServerData.hpp"
#include "../parseRequest/Request.hpp"
#include <netinet/in.h>
#include <netinet/ip.h>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <sys/errno.h>
#include <unistd.h>
#include <cstdio>

#define BUFF_SIZE 1024
typedef std::map<int, Request> RequestMap; // DUPLICATED
void        isFdReady(int fd);
int			createServerSocket(const ServerData& Data);
bool        sendingResponse(Request & req);
void		acceptConnection(RequestMap &request, int &max_fd, fd_set *current_sockets, const int i);
void		closeConnection(int fd, std::map<int, Request> & request ,fd_set *currentSocketsSet);