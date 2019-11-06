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
    printf("Server\n");
    //sleep(15);
    char* pid = (char*)calloc(6, sizeof(char));
    while(1)
    {
        int fd = open("mypipe.p", O_RDONLY);
        read(fd, pid, 5 * sizeof(char));

        if (argc < 2)
        {
            printf("Nothing to write, try again\n");
            exit(EXIT_FAILURE);
        }
        if (argc > 2)
        {
            printf("Too many arguments, try again\n");
            exit(EXIT_FAILURE);
        }
        FILE* file = fopen(argv[1], "r");
        if (file == nullptr)
        {
            printf("Unable to open file %s, try again\n", argv[1]);
            exit(EXIT_FAILURE);
        }

	    fseek(file, 0, SEEK_END);
        int size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char* buf = (char*)calloc(size + 5, sizeof(char));
        fread(buf, sizeof(char), size, file);

        int reply = open(pid, O_WRONLY);
	    write(reply, buf, size * sizeof(char));

	    free(buf);
    }

    return 0;
}