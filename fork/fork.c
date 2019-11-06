#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

int global = 0;

int getn (int argc, char* argv[]);
void fork_p(int n);

void* function (void* n)
{
    int* temp = (int*) n;
    int j = 0;
    for (; j < *temp; j++)
        global++;

    //printf("id %d current is %d\n", getpid(), global);
}

int main(int argc, char *argv[])
{
    int n = getn(argc, argv);
    fork_p(n);

    /*pid_t temp = fork();
    if (temp == 0)
    {
        execv("res", argv);
    } */

    /*pthread_t* array = (pthread_t*)calloc(n, sizeof(pthread_t));

    void * restrict m = (void* restrict)(&n);
    int i = 0;
    for (; i < n; i++)
    {
        pthread_t temp;
        pthread_create(&temp, NULL, &function, m);
        array[i] = temp;
    }

    for (i = 0; i < n; i++)
    {
        pthread_join(array[i], NULL);
    } */

    //printf("%d\n", global); 

    return 0;
}

int getn (int argc, char* argv[])
{
    int base;
           char *endptr, *str;
           long val;

           if (argc < 2) {
               fprintf(stderr, "Usage: %s str [base]\n", argv[0]);
               //exit(EXIT_FAILURE);
           }

           str = argv[1];
           base = (argc > 2) ? atoi(argv[2]) : 10;

           errno = 0;   
           val = strtol(str, &endptr, base);

           if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                   || (errno != 0 && val == 0)) {
               perror("strtol");
               //exit(EXIT_FAILURE);
           }

           if (endptr == str) {
               fprintf(stderr, "No digits were found\n");
               //exit(EXIT_FAILURE);
           }

           printf("strtol() returned %ld\n", val);

           if (*endptr != '\0')
               printf("Further characters after number: %s\n", endptr);
    return val;
           //exit(EXIT_SUCCESS);
}

void fork_p(int n)
{
    int number = 0;

    printf("parent id %d \n", getpid());

    for (number = 0; number < n; number++)
    {
        pid_t temp = fork();

        if (temp == 0)
        {
            printf("number %d pid %d\n", number, getpid());
            break;
        }
    }
}