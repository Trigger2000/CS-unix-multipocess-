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
#include <sys/shm.h>

int main(int argc, char** argv)
{
    key_t key = ftok("key_file", 10);

    FILE* file = fopen(argv[1], "r");
    if (file == nullptr)
    {
        printf("Unable to open file %s, try again\n", argv[1]);
        exit(EXIT_FAILURE);
    }
	fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int mem = shmget(key, size + 5, 0777 | IPC_CREAT);
    char* membuf = (char*)shmat(mem, NULL, 0);
    printf("%s\n", membuf);

    shmdt(membuf);

    shmctl(mem, IPC_RMID, NULL);
}