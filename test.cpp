#include <stdio.h>
#include <netdb.h>


int main(int argc, char *argv[])
{
    struct hostent *lh = gethostbyname("kubernetes.docker.internal");

    if (lh)
        puts(lh->h_addr_list[0]);
    else
        herror("gethostbyname");

    return 0;
}