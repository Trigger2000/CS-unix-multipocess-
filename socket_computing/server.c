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

struct connection_info
{
    struct sockaddr_in6 info;
    int size;
};

typedef struct connection_info coinf;

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Incorrect input\n");
        exit(EXIT_FAILURE);
    }

    int workers_amount = strtol(argv[1], NULL, 10);
    if (workers_amount < 1)
    {
        printf("Incorrect amount of workers\n");
        exit(EXIT_FAILURE);
    }

    int udp_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    if (udp_socket == -1)
    {
        printf("Can't make a socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in6 info;
    memset(&info, 0, sizeof(struct sockaddr_in6));
    info.sin6_family = AF_INET6;
    info.sin6_addr = in6addr_any;
    info.sin6_port = htons(PORT_NUM);

    if (bind(udp_socket, (struct sockaddr*)&info, sizeof(struct sockaddr_in6)) == -1)
    {
        printf("Can't bind\n");
        exit(EXIT_FAILURE);
    }

    coinf* workers_info = (coinf*)calloc(workers_amount, sizeof(coinf));
    for (int i = 0; i < workers_amount; ++i)
    {
        char buf[READY_SIZE];
        workers_info[i].size = sizeof(struct sockaddr_in6);
        int bytes = recvfrom(udp_socket, buf, READY_SIZE, 0, (struct sockaddr*)(&workers_info[i].info), 
                            &workers_info[i].size);
        
        if (bytes == -1)
        {
            printf("Error occurred on the worker's side\n");
            exit(EXIT_FAILURE);
        }

        printf("%s\n", buf);
    }


    return 0;
}