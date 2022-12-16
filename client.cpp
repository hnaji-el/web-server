#include <strings.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h> // read(), write(), close()
#include<netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <vector>

#define BUFF_SIZE 10
#define MAX 80
#define PORT 3333
#define SA struct sockaddr
class client
{
    public:
        int port;
        int sock_fd;
        struct sockaddr_in  addr;
        std::string request;
    public:
        client(int port, std::string request) : port(port), request(request)
        {
            bzero(&this->addr, sizeof(this->addr));
            this->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (this->sock_fd < 0)
            {
                std::cout << "FAILED SOCKET CREATION" << std::endl;
                exit(1);
            }
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            addr.sin_port = htons(this->port);
            if (connect(this->sock_fd, (SA*)&addr, sizeof(addr)) != 0)
            {
                std::cout << "FAILED CONNECTION" << std::endl;
                exit(1);
            }
            else {
                std::cout << "client with port " << this->port << " connected" << std::endl;
            }
        }
};

int main()
{
    std::vector<client> clients;
    for (int i = 0; i < 3; i++)
        clients.push_back(client(PORT, std::string(100, i + 48)));
    char buff[BUFF_SIZE];

    while(1)
    {
        for (int i = 0; i < 3; i++)
        {
            // std::cout << clients[i].request << std::endl;
            write(clients[i].sock_fd, clients[i].request.c_str(), sizeof(clients[i].request));
            read(clients[i].sock_fd, buff, sizeof(buff));
            printf("%s\n",buff);
        }
    }
    // client a(PORT, "a");

    // bzero(str, sizeof(str));
    // read(c_socket, str, sizeof(str));
    // printf("%s\n",str);
}