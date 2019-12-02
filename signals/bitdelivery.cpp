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

int globalbit = 0;
int check = 0;

void sendmsg (char buf, sigset_t* set);
void rcvmsg (int chldpid, sigset_t* set);

void setzero (int signum)
{
    printf("z\n");
    globalbit = 0;
}

void setone (int signum)
{
    //printf("o\n");
    globalbit = 1;
}

void setcheck (int signum)
{
    //printf("c\n");
    check = 1;
}

void handle (int signum)
{
    printf("h\n");
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
    sigdelset (&set, SIGUSR1);
    sigdelset (&set, SIGUSR2);
    sigdelset (&set, SIGHUP);
    sigdelset (&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, NULL);

    struct sigaction actzero, actone, actchld, actmsg;
    actone.sa_handler = setone;
    actzero.sa_handler = setzero;
    actchld.sa_handler = setcheck;
    actmsg.sa_handler = handle;
    
    sigaction(SIGUSR1, &actzero, NULL);
    sigaction(SIGUSR2, &actone, NULL);
    sigaction(SIGCHLD, &actchld, NULL);
    sigaction(SIGHUP, &actmsg, NULL);

    pid_t pid = fork();
    if (pid == 0)
    {
        char buf = 0;
        while (read(file, &buf, 1) != 0)
        {
            //printf("child\n");
            sendmsg(buf, &set);
            buf = 0;
        }

        //kill(getppid(), SIGCHLD);
    }
    else
    {
        rcvmsg(pid, &set);
        wait(0);
    }
    

    exit(EXIT_SUCCESS);
}

void sendmsg (char buf, sigset_t* set)
{
    for (int i = 0; i < 8; ++i)
    {
        int bit = buf & 128;
        bit = bit >> 7;
        buf = buf << 1;
        
        //sigsuspend(set);
        if (bit == 0)
        {
            kill(getppid(), SIGUSR1);
        }
        else
        {
            kill(getppid(), SIGUSR2);
        }
        //printf("bit\n");
        sigsuspend(set);
        //printf("scdcf");
    }
}

void rcvmsg (int chldpid, sigset_t* set)
{
    while(1)
    {
        char buf = 0;
        for (int i = 0; i < 8; ++i)
        {
            sigsuspend(set);
            //printf("entr\n");
            buf = buf << 1;
            buf = buf | globalbit;
            kill(chldpid, SIGHUP);

            if (check == 1)
                break;
        }

        //printf("sass\n");
        //printf("%c\n", buf);
        write(1, &buf, 1);

        if (check == 1)
            break;
    }
}