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
    printf("Client\n");
    pid_t pid = getpid();
    char* local_fifo = (char*)calloc(6, sizeof(char));
    sprintf(local_fifo, "%d", pid);
    mkfifo(local_fifo, 0777);

    sleep(15);
    int fd = open("mypipe.p", O_WRONLY);
    write(fd, local_fifo, 5 * sizeof(char));
    int reply = open(local_fifo, O_RDONLY);

    char* buf = (char*)calloc(101, sizeof(char));
    int amount = read(reply, buf, 100 * sizeof(char));
    while (amount == 100)
    {
        printf("%s", buf);
        amount = read(reply, buf, 100 * sizeof(char));
    }
	for (int i = 0; i < amount; i++)
    {
        printf("%c", buf[i]);
    }
	printf("\n");

    unlink(local_fifo);
	free(buf);
    free(local_fifo);

    return 0;
}