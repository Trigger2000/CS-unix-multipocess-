#include "declarations.h"

int getcpuinfo()
{
    FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo == NULL)
    {
        printf("Can't open /proc/cpunfo\n");
        exit(EXIT_FAILURE);
    }
    
    char* lineptr = NULL;
    size_t n = 0;
    for (int i = 0; i < 13; ++i)
    {
        getline(&lineptr, &n, cpuinfo);
        if (i != 12)
        {
            free(lineptr);
            n = 0;
        }
    }

    char* ptr = strchr(lineptr, ':');
    ptr += 2*sizeof(char);
    return strtol(ptr, NULL, 10);
}