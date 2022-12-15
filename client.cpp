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
#define MAX 80
#define PORT 3333
#define SA struct sockaddr
int main()
{
    struct sockaddr_in  addr;

    int c_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c_socket < 0)
    {
        std::cout << "FAILED SOCKET CREATION" << std::endl;
        exit(1);
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(PORT);
    if (connect(c_socket, (SA*)&addr, sizeof(addr)) != 0)
    {
        std::cout << "FAILED CONNECTION" << std::endl;
        exit(1);
    }
    char str[13] = "HELLO WORLD!";
    write(c_socket, str, sizeof(str));
    bzero(str, sizeof(str));
    read(c_socket, str, sizeof(str));
    printf("%s\n",str);
}