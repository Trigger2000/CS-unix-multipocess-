#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>

#define PORT_NUM 54321
#define READY_SIZE 6 //capacity for word "ready"

int main()
{
    int worket = socket(AF_INET6, SOCK_DGRAM, 0);
    if (worket == -1)
    {
        printf("Worker can't create a socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in6 info;
    memset(&info, 0, sizeof(struct sockaddr_in6));
    info.sin6_family = AF_INET6;
    info.sin6_addr = in6addr_any;
    info.sin6_port = htons(PORT_NUM);

    char msg[6] = "ready";

    int tmp = sendto(worket, msg, 6, 0, (struct sockaddr*)&info, sizeof(struct sockaddr_in6));


    return 0;
}