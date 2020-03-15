#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define func(x) ((x)*(x))

double* arr;

struct thread_info
{
    double begin;
    double end;
    int num;
};
typedef struct thread_info thread_info;

void* calculate(void* arg)
{
    thread_info* tmp = (thread_info*)arg;
    double cur = tmp->begin;
    double val = 0.0;
    while (cur < tmp->end)
    {
        val += func(cur) * 0.000001;
        cur += 0.000001;
    }
    arr[tmp->num] = val;

    return NULL;
}

int main(int argc, char** argv)
{
    int begin = 0, end = 1000, n = strtol(argv[1], NULL, 10);
    if (n < 1)
    {
        exit(EXIT_FAILURE);
    }

    double diff = (end - begin) / n;
    double cur = diff;
    arr = (double*)calloc(n, 64);
    pthread_t* threads = (pthread_t*)calloc(n, sizeof(pthread_t));
    thread_info* info = (thread_info*)calloc(n, 64);

    for (int i = 1; i < n; ++i)
    {
        info[i].begin = cur;
        info[i].end = cur + diff;
        info[i].num = i;
        pthread_create(&threads[i], NULL, &calculate, &info[i]);
        cur += diff;
    }

    thread_info first = {begin, diff, 0};
    calculate(&first);
    double result = arr[0];

    for (int i = 1; i < n; ++i)
    {
        pthread_join(threads[i], NULL);
        result += arr[i];
    }

    //printf("%lf\n", result);

    free(arr);
    free(threads);
    free(info);

    return 0;
}