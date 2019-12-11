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
#include <sys/select.h>

struct temp
{
    int fd[2];
};

int main(int argc, char** argv)
{
    pid_t pid = 0;
    temp* ar = (temp*)calloc(5, sizeof(temp));
    int i = 0;
    int fd[2];
    for (i = 0; i < 5; ++i)
    {
        pipe(fd);
        pid = fork();
        if (pid == 0)
        {
            break;
        }
        else
        {
            temp t;
            t.fd[0] = fd[0];
            t.fd[1] = fd[1];
            ar[i] = t;
        }
            
    }

    if (pid == 0)
    {
        printf("child %d: 0 = %d, 1 = %d\n", i, fd[0], fd[1]);
        for (int j = 0; j < 5; ++j)
        {
            printf("%d %d\n", ar[j].fd[0], ar[j].fd[1]);
        }
    }
    else
    {
        for (int j = 0; j < 5; ++j)
        {
            wait(0);
        }
        printf("parent\n");
        for (int j = 0; j < 5; ++j)
        {
            printf("%d %d\n", ar[j].fd[0], ar[j].fd[1]);
        }
    }
    

    return 0;
}