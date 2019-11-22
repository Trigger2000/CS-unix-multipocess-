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
    char* pid = (char*)calloc(6, sizeof(char));
    int fd = open("mypipe.p", O_RDONLY);
    read(fd, pid, 5 * sizeof(char));

    if (argc != 2)
    {
        printf("Incorrect arguments\n");
        int reply = open(pid, O_WRONLY);
        char sent[20] = "Nothing to read";
        write(reply, &sent, 20 * sizeof(char));
        exit(EXIT_FAILURE);
    }
    else
    {
        int file = open(argv[1], O_RDONLY);
        if (file == -1)
        {
            printf("Unable to open file %s, try again\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        int reply = open(pid, O_WRONLY);
        char* buf = (char*)calloc(101, sizeof(char));
        int amount = read(file, buf, 100);

        while (amount != 0)
        {
            //sleep(100);
	        write(reply, buf, amount);
            amount = read(file, buf, 100);
        } 
    }
    return 0;
}
