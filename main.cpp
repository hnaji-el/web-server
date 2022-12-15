#include <strings.h>
#include <sys/_endian.h>
#include <sys/socket.h>
// #include <sys/_types/_socklen_t.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include<iostream>
#define PORT 3333
// #define PORT 222
#define SA struct sockaddr

int main()
{
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t cli_addr_len;
    int sock_fd,conn_fd;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1){
		printf("socket creation failed...\n");
		exit(0);
	}
    else
	    printf("Socket successfully created..\n");
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    if ((bind(sock_fd, (SA*)&serv_addr, sizeof(serv_addr))) != 0)
    {
        std::cout << "FAILED BIND" << std::endl;
        exit(1);
    }
    if ((listen(sock_fd, 5)) != 0)
    {
        std::cout << "FAILED LISTEN" << std::endl;
        exit(1);
    }
    printf("WAITING FOR ACCEPT..\n");
    cli_addr_len = sizeof(cli_addr);
    conn_fd = accept(sock_fd, (SA*)&cli_addr, &cli_addr_len);
    printf("CONNECTION ESTABLISHED");

    return 0;
}