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
#include <sys/prctl.h>

int globalbit = 0;

void setzero (int signum)
{
    globalbit = 0;
}

void setone (int signum)
{
    globalbit = 1;
}

void finish (int signum)
{
    exit(EXIT_SUCCESS);
}

void handle (int signum) {};

int main(int argc, char** argv)
{
    int file = open(argv[1], O_RDONLY);
    if (file == -1)
    {
        printf("Unable to open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    sigset_t set, old_set;
    sigemptyset(&set);
    sigemptyset(&old_set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, &old_set);

    struct sigaction actzero, actone, actchld, actmsg;
    actone.sa_handler = setone;
    actzero.sa_handler = setzero;
    actchld.sa_handler = finish;
    actmsg.sa_handler = handle;
    actone.sa_flags = SA_RESTART;
    actzero.sa_flags = SA_RESTART;
    actchld.sa_flags = SA_RESTART;
    actmsg.sa_flags = SA_RESTART;
    
    sigaction(SIGUSR1, &actzero, NULL);
    sigaction(SIGUSR2, &actone, NULL);
    sigaction(SIGCHLD, &actchld, NULL);
    sigaction(SIGHUP, &actmsg, NULL);

    pid_t parent_id = getpid();
    pid_t pid = fork();

    if (pid == 0)
    {
        prctl(PR_SET_PDEATHSIG, SIGCHLD);

        if (getppid() != parent_id)
        {
            exit(EXIT_FAILURE);
        }

        sigset_t setch, old_setch;
        sigemptyset(&setch);
        sigaddset(&setch, SIGHUP);
        sigprocmask(SIG_BLOCK, &setch, &old_setch);

        char buf = 0;
        while (read(file, &buf, 1) != 0)
        {
            for (int i = 0; i < 8; ++i)
            {
                int bit = buf & 128;
                bit = bit >> 7;
                buf = buf << 1;
                //sleep(1);
                if (bit == 0)
                {
                    kill(getppid(), SIGUSR1);
                }
                else
                {
                    kill(getppid(), SIGUSR2);
                }

                sigsuspend(&old_set);
            }

            buf = 0;
        }
    }
    else
    {
        while(1)
        {
            char buf = 0;
            for (int i = 0; i < 8; ++i)
            {
                sigsuspend(&old_set);
                buf = buf << 1;
                buf = buf | globalbit;
                kill(pid, SIGHUP);
            }

            write(STDOUT_FILENO, &buf, 1);
        }
    }

    exit(EXIT_SUCCESS);
}