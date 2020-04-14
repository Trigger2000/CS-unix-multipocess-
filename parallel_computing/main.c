#include "declarations.h"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("No input\n");
        exit(EXIT_FAILURE);
    }

    int threads_requested = strtol(argv[1], NULL, 10);
    int cpu_cores = getcpuinfo();
    calculate(cpu_cores, threads_requested);

    return 0;
}