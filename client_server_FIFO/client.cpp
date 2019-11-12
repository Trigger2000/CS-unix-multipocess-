#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv)
{    
    pid_t pid = getpid();
    char* local_fifo = (char*)calloc(6, sizeof(char));
    sprintf(local_fifo, "%d", pid);
    mkfifo(local_fifo, 0777);

    int fd = open("mypipe.p", O_WRONLY | O_NONBLOCK);
    write(fd, local_fifo, 5 * sizeof(char));

    int reply = open(local_fifo, O_RDONLY | O_NONBLOCK);
    sleep(1);

    char buf = 0;
    while (read(reply, &buf, 1))
    {
        printf("%c", buf);
        buf = 0;
    } 

    unlink(local_fifo);
    free(local_fifo);

    return 0;
}