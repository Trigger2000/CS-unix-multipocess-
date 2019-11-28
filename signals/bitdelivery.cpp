#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define SENDZERO SIGUSR1
#define SENDONE SIGUSR2
#define MSGRCVD SIGHUP

int globalbit = 0;
int check = 0;

void sendmsg (char buf, sigset_t* set);
void rcvmsg (int chldpid, sigset_t* set);

void setzero (int signum)
{
    globalbit = 0;
}

void setone (int signum)
{
    globalbit = 1;
}

void setcheck (int signam)
{
    check = 1;
}

int main(int argc, char** argv)
{
    int file = open(argv[1], O_RDONLY);
    if (file == -1)
    {
        printf("Unable to open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    sigset_t set;
    sigfillset (&set);
    sigdelset (&set, SIGCHLD);
    sigdelset (&set, SENDZERO);
    sigdelset (&set, SENDONE);
    sigdelset (&set, MSGRCVD);
    sigdelset (&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, NULL);

    struct sigaction actzero, actone, actchld, actmsg;
    actone.sa_handler = setone;
    actzero.sa_handler = setzero;
    actchld.sa_handler = setcheck;
    actmsg.sa_handler = SIG_IGN;
    
    sigaction(SENDZERO, &actzero, NULL);
    sigaction(SENDONE, &actone, NULL);
    sigaction(SIGCHLD, &actchld, NULL);
    sigaction(MSGRCVD, &actmsg, NULL);

    pid_t pid = fork();
    if (pid == 0)
    {
        char buf = 0;
        while (read(file, &buf, 1) != 0)
        {
            sendmsg(buf, &set);
        }

        kill(getppid(), SIGCHLD);
    }
    else
    {
        rcvmsg(pid, &set);
        wait(0);
    }
    

    return 0;
}

void sendmsg (char buf, sigset_t* set)
{
    for (int i = 0; i < 8; ++i)
    {
        int bit = buf & 128;
        bit = bit >> 7;
        buf = buf << 1;

        if (bit == 0)
        {
            kill(getppid(), SENDZERO);
        }
        else
        {
            kill(getppid(), SENDONE);
        }

        sigsuspend(set);
    }
}

void rcvmsg (int chldpid, sigset_t* set)
{
    while(1)
    {
        char buf = 0;
        for (int i = 0; i < 8; ++i)
        {
            printf("dafe\n");
            sigsuspend(set);
            printf("entr\n");
            buf = buf << 1;
            buf = buf | globalbit;
            kill(chldpid, MSGRCVD);

            if (check == 1)
                break;
        }

        printf("sass\n");
        write(1, &buf, 1);

        if (check == 1)
            break;
    }
}