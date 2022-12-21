 #include "server.hpp"
server::server(int port) :port(port)
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
