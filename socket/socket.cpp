#include "socket.hpp"

void	acceptConnection(RequestMap &request, int &max_fd, fd_set *current_sockets, const int i)
{
	std::cout << "CONNECTION" << std::endl;
	const int	conn_fd = accept(i, NULL, NULL);
	if (fcntl(conn_fd, F_SETFL, O_NONBLOCK) < 0)
		exit(EXIT_FAILURE);
    int option_value = 1;
    setsockopt(conn_fd, SOL_SOCKET, SO_NOSIGPIPE, &option_value, sizeof(option_value));
	max_fd = (conn_fd > max_fd) ? conn_fd : max_fd;
	request[conn_fd] = Request(conn_fd);
	FD_SET(conn_fd, current_sockets);
}
void    sendingResponse(Request & req)
{
	char buff[BUFF_SIZE + 1];
	bzero(&buff, BUFF_SIZE);
	int r = read(req.fdBody, buff, BUFF_SIZE);
	write(req.fd, buff, r);
    if (r < BUFF_SIZE)
        req.resState =BODYSENT;
    else
        req.resState =BODYNOTSENT;  
}

void	closeConnection(int fd, std::map<int, Request> & request ,fd_set *currentSocketsSet)
{
	std::cout << "connection with fd " << fd << " is going to be closed" << std::endl;
	close(fd);
	request.erase(fd);
	FD_CLR(fd, currentSocketsSet);
}
int createServerSocket(const ServerData& Data)
{
    int sock_fd;
    struct sockaddr_in serv_addr;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        std::cerr << "reuseaddr error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int a = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int)) < 0)
    {
        std::cerr << "reuseaddr error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "nonblock error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(Data.host.c_str());
    serv_addr.sin_port = htons(Data.port);
    printf("PORT:%d\n", Data.port);
    if ((bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) != 0)
    {
        std::cerr << "bind error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if ((listen(sock_fd, 1024)) != 0)
    {
        std::cerr << "listen error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return sock_fd;
}
