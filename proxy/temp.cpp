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
#include <dirent.h>
#include <string.h>

int main(int argc, char** argv)
{
    char* temp = (char*)calloc(10, 1);
    scanf("%s", temp);

    printf("%d\n", strlen(temp));

    memset(temp, 0, 10);

    printf("%s %d\n", temp, strlen(temp));

    return 0;
}