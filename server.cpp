#include "server.hpp"
#include <cstdio>
server::server(const ServerData& Data)
{
    struct sockaddr_in serv_addr;
    this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->sock_fd == -1)
    {
        std::cerr << "reuseaddr error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    int a = 1;
    if (setsockopt(this->sock_fd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int)) < 0)
    {
        std::cerr << "reuseaddr error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(this->sock_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        std::cerr << "nonblock error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(Data.host.c_str());
    serv_addr.sin_port = htons(Data.port);
    printf("PORT:%d\n", Data.port);
    if ((bind(this->sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) != 0)
    {
        std::cerr << "bind error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    if ((listen(this->sock_fd, 1024)) != 0)
    {
        std::cerr << "listen error " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}
