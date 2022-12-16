#include <string>

#include <strings.h>
// #include <sys/_endian.h>
#include <sys/socket.h>
// #include <sys/_types/_socklen_t.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/stat.h>
#include <unistd.h> // read(), write(), close()

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

int main() {
  struct sockaddr_in serv_addr, cli_addr;
  socklen_t cli_addr_len;
  int sock_fd, conn_fd;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT);

  if ((bind(sock_fd, (SA *)&serv_addr, sizeof(serv_addr))) != 0) {
    std::cout << "FAILED BIND" << std::endl;
    exit(1);
  }
  if ((listen(sock_fd, 5)) != 0) {
    std::cout << "FAILED LISTEN" << std::endl;
    exit(1);
  }

  // char *buff;

  // buff = (char*)malloc(100 * sizeof(buff));
  char buff[BUFF_SIZE];
  printf("WAITING FOR ACCEPT..\n");
  cli_addr_len = sizeof(cli_addr);
  conn_fd = accept(sock_fd, (SA *)&cli_addr, &cli_addr_len);
  request r(conn_fd);
  // int file_fd = open(r.file_name.c_str(), O_RDWR | O_APPEND | O_CREAT, 0666);
  while (1) {
    bzero(buff, BUFF_SIZE);
    // printf("CONNECTION ESTABLISHED\n");
    if (read(conn_fd, buff, BUFF_SIZE) < 0) {
      printf("ERROR READ");
      exit(1);
    }
    char hello[100] = "HTTP/1.1 200 OK\nContent-Type: "
                      "text/plain\nContent-Length: 12\n\nHello world!";
    if (std::string(buff).find("lll") != std::string::npos) {
      // printf("%s\n",buff);

      write(conn_fd, hello, strlen(hello));
      exit(1);
    }
    // printf("%s\n",buff);
    // write(file_fd, buff, BUFF_SIZE);
  }

  return 0;
}