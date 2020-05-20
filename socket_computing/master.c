#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#define WORKER_UDP_PORT 5000
#define SUPERVISOR_MAIN_TCP_PORT 5001

struct calc_segm_arg_t
{
    double begin;
    double end;
};

typedef struct calc_segm_arg_t calc_segm_arg_t;

extern int errno;

int udp_broadcast();
double serve_workers(int main_tcp_sock_fd, double begin, double end, unsigned int workers_num);

struct tcp_conn_t
{
    struct sockaddr_in sock_in;
    bool received;
    bool sent;
    bool accepted;
    int sock_fd;
    int threads_num;
    calc_segm_arg_t arg;
};

typedef struct tcp_conn_t tcp_conn_t;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: master <workers_num>\n");
        return -1;
    }
    
    long int workers_num = strtol(argv[1], NULL, 10);
    double begin = 1.0;
    double end = 1000.0;

    int main_tcp_sock_fd = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    
    int optval = 1;
    setsockopt(main_tcp_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in main_tcp_sock_in = {0};
    
    main_tcp_sock_in.sin_addr.s_addr = htonl(INADDR_ANY);
    main_tcp_sock_in.sin_port = htons(SUPERVISOR_MAIN_TCP_PORT);
    main_tcp_sock_in.sin_family = AF_INET;
    
    bind(main_tcp_sock_fd, (struct sockaddr *)&main_tcp_sock_in, sizeof(struct sockaddr_in));
    
    listen(main_tcp_sock_fd, workers_num);

    //printf("Looking for workers...\n");
    errno = 0;
    if (udp_broadcast() != 0)
    {
        perror(NULL);
        return -1;
    }

    printf("Result: %lg\n", serve_workers(main_tcp_sock_fd, begin, end, workers_num));

    return 0;
}

double serve_workers(int main_tcp_sock_fd, double begin, double end, unsigned int workers_num)
{
    fd_set read_fds, write_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_SET(main_tcp_sock_fd, &read_fds);

    int optval = 1;
    int keepidle = 5;
    int keepintvl = 1;
    int keepcnt = 5;
    
    double result = 0.;
    double cur_begin = begin;

    int workers_threads_num_known = 0;
    int total_threads_num = 0;

    long int accepted = 0;
    long int result_received = 0;

    char str_addr[INET_ADDRSTRLEN] = {0};

    socklen_t tcp_sock_in_len = sizeof(struct sockaddr_in);
    struct tcp_conn_t *workers = (struct tcp_conn_t *)calloc(workers_num, sizeof(tcp_conn_t)); 

    while(1)
    {
        select(FD_SETSIZE, &read_fds, &write_fds, NULL, NULL);

        if (FD_ISSET(main_tcp_sock_fd, &read_fds))
        {
            if (accepted == workers_num)
            {
                //printf("Excess worker tried to connect. Connection will be refused.\n");
                int sock_fd = accept(main_tcp_sock_fd, NULL, NULL);
                close(sock_fd);
            }
            else 
            {
                errno = 0;
                workers[accepted].sock_fd = accept4(main_tcp_sock_fd, (struct sockaddr *)&workers[accepted].sock_in, &tcp_sock_in_len, SOCK_NONBLOCK);
                if (workers[accepted].sock_fd == -1)
                {
                    perror(NULL);
                    exit(-1);
                }
    
                setsockopt(workers[accepted].sock_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
                setsockopt(workers[accepted].sock_fd, SOL_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
                setsockopt(workers[accepted].sock_fd, SOL_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
                setsockopt(workers[accepted].sock_fd, SOL_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
             
                inet_ntop(AF_INET, &(workers[accepted].sock_in.sin_addr), str_addr, INET_ADDRSTRLEN);
                //printf("Worker at %s:%d connected.\n", str_addr, ntohs(workers[accepted].sock_in.sin_port));
                
                workers[accepted].accepted = true;
                workers[accepted].sent     = false;
                workers[accepted].received = false;
                workers[accepted].threads_num = 0;

                if (++accepted == workers_num)
                    close(main_tcp_sock_fd);
            }
        }

        for (unsigned int i = 0; i < workers_num; ++i)
        {
            if (FD_ISSET(workers[i].sock_fd, &write_fds))
            {
                workers[i].arg = (struct calc_segm_arg_t){
                    .begin = cur_begin,
                    .end = cur_begin + workers[i].threads_num * (end - begin) / total_threads_num
                };
                cur_begin = workers[i].arg.end;
                if (send(workers[i].sock_fd, &workers[i].arg, sizeof(calc_segm_arg_t), 0) != sizeof(calc_segm_arg_t))
                {
                    printf("Error sending args.\n");
                    exit(-1);
                }
                workers[i].sent = true;
            }
            else if (FD_ISSET(workers[i].sock_fd, &read_fds))
            {
                if (workers_threads_num_known < workers_num)
                {
                    if (recv(workers[i].sock_fd, &workers[i].threads_num, sizeof(int), 0) != sizeof(int))
                    {
                        printf("Error receiving threads_num.\n");
                        exit(-1);
                    }
                    ++workers_threads_num_known;
                    total_threads_num += workers[i].threads_num;
                }
                else
                {
                    double worker_result = 0.;
                    if (recv(workers[i].sock_fd, &worker_result, sizeof(double), 0) != sizeof(double))
                    {
                        printf("Error receiving result.\n");
                        exit(-1);
                    }
                    workers[i].received = true;
                    result += worker_result;
                    ++result_received;
                    close(workers[i].sock_fd); 
                }
            }
        }

        if (result_received == workers_num)
            break;
        
        FD_ZERO(&write_fds);
        FD_ZERO(&read_fds);
        
        for (unsigned int i = 0; i < workers_num; ++i)
        {
            if (!workers[i].sent && workers[i].accepted && (workers_threads_num_known == workers_num))
                FD_SET(workers[i].sock_fd, &write_fds);
    
            if ((!workers[i].received && workers[i].accepted && (workers_threads_num_known == workers_num)) || ((workers_threads_num_known < workers_num) && workers[i].accepted))
                FD_SET(workers[i].sock_fd, &read_fds);
        }
            
        if (accepted != workers_num)
            FD_SET(main_tcp_sock_fd, &read_fds);
    }

    free(workers);

    return result;
}


int udp_broadcast()
{
    int sock_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock_fd == -1)
        return -1;

    int optval = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) != 0)
        return -1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
        return -1;
    
    struct sockaddr_in sock_in = {0};
    
    sock_in.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    sock_in.sin_port = htons(WORKER_UDP_PORT);
    sock_in.sin_family = AF_INET;

    char buffer[1] = "";
    if (sendto(sock_fd, buffer, 1, 0, (struct sockaddr *)&sock_in, sizeof(struct sockaddr_in)) != 1)
        return -1;

    close(sock_fd);
    
    return 0;
}