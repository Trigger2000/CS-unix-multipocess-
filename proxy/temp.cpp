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

int main(int argc, char** argv)
{
    int fd[2];
    pipe(fd);
    fd_set tmp1, tmp2;
    FD_ZERO(&tmp1);
    FD_ZERO(&tmp2);
    FD_SET(fd[0], &tmp1);
    FD_SET(fd[1], &tmp2);
    int max = 0;
    if (fd[0] > fd[1])
    {
        max = fd[0];
    }
    else
    {
        max = fd[1];
    }

    close(fd[1]);
    
    char c = 'c';
    write(fd[1], &c, 1);
    int res = read(fd[0], NULL, 1);
    printf("res is %d\n", res);
    char buf;
    res = read(fd[0], &buf, 1);
    printf("res is %d\n", res);
    /*char buf;
    close(fd[1]);
    int res = select(fd[0] + 1, &tmp1, NULL, NULL, NULL);
    printf("%d %d\n", res, read(fd[0], &buf, 1));

    res = select(fd[1] + 1, NULL, &tmp2, NULL, NULL);
    printf("%d\n", res); */

    return 0;
}