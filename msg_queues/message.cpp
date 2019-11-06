#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <limits.h>
#include <sys/wait.h>
//#include <iostream>

#define MAX 10200

/*struct msgbuf 
{
    long mtype;
    char mtext;
}; */

int getn (int argc, char* argv[]);
void send_msg(int qid, int msgtype);
void get_msg(int qid, int msgtype);

int main(int argc, char* argv[])
{   
    //printf("%d\n", getpid());
    int n = getn(argc, argv);
    if (n >= MAX)
    {
        printf("incorrect n\n");
        exit(EXIT_FAILURE);
    }

    errno = 0;
    int qid = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);
    if (errno != 0)
    {
        printf("msgget failed\n");
        exit(EXIT_FAILURE);
    }

    pid_t id;
    int i = 0;
    for (i = 1; i <= n; i++)
    {
        id = fork();
        if (id == 0)
            break;
    }

    if (id == 0)
    {
        if (i == 1)
        {
            get_msg(qid, n);
            printf("%d ", i);
            fflush(stdout);
            send_msg(qid, i);
        }
        else if (i == n)
        {
            send_msg(qid, i);
            get_msg(qid, i - 1);
            printf("%d\n", i);
            fflush(stdout);
            send_msg(qid, n + 1);
        }
        else
        {
            get_msg(qid, i - 1);
            printf("%d ", i);
            fflush(stdout);
            send_msg(qid, i);
        }
    }
    else
    {
        get_msg(qid, n + 1);
    }
    

    
    return 0;
}

int getn (int argc, char* argv[])
{
    int base;
    char *endptr, *str;
    long val;

    if (argc < 2) 
    {
       fprintf(stderr, "Usage: %s str [base]\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    str = argv[1];
    base = (argc > 2) ? atoi(argv[2]) : 10;

    errno = 0;   
    val = strtol(str, &endptr, base);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) 
    {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == str) 
    {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }

    if (*endptr != '\0')
    {
        printf("Further characters after number: %s\n", endptr);
        exit(EXIT_FAILURE);
    }

    return val;
}

void send_msg(int qid, int msgtype)
{
    errno = 0;
    struct msgbuf msg;
    msg.mtype = msgtype;

    msgsnd(qid, (void*)&msg, 0, 0);
    if (errno != 0)
    {
        printf("msgsnd error\n");
        exit(EXIT_FAILURE);
    }
}

void get_msg(int qid, int msgtype)
{
    errno = 0;
    struct msgbuf msg;
    msgrcv(qid, (void*)&msg, 0, msgtype, 0);
    if (errno != 0)
    {
        printf("msgrcv error\n");
        exit(EXIT_FAILURE);
    }
}