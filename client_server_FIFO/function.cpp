#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int compare (int fd1, int fd2)
{
    struct stat fs1, fs2;
    fstat(fd1, &fs1);
    fstat(fd2, &fs2);

    if (fs1.st_dev == fs2.st_dev && fs1.st_ino == fs2.st_ino)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}

int main()
{

    return 0;
}