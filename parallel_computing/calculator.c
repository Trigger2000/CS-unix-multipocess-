#include "declarations.h"

double* arr;
double diff = 0;

double calculate(int cpu_cores, int threads_requested)
{
    cpu_set_t cpui;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    CPU_ZERO(&cpui);
    CPU_SET(0, &cpui);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpui);

    diff = (END - BEGIN) / ((double)threads_requested);
    int size = threads_requested > cpu_cores * 2? threads_requested : cpu_cores * 2;
    arr = (double*)calloc(threads_requested, 64);
    pthread_t* threads = (pthread_t*)calloc(size, sizeof(pthread_t));
    int* data = (int*)calloc(threads_requested, sizeof(int));

    int i = 1;
    for (i = 1; i < threads_requested; ++i)
    {
        pthread_attr_destroy(&attr);
        pthread_attr_init(&attr);
        CPU_ZERO(&cpui);
        CPU_SET(i % (cpu_cores * 2), &cpui);
        pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpui);
        data[i] = i;
        int a = pthread_create(&threads[i], &attr, &integrate, &data[i]);
        if (a != 0)
        {
            printf("Can't create thread %d\n", i);
            break;
        }
    }

    for (i = threads_requested; i < cpu_cores * 2; ++i)
    {
        pthread_attr_destroy(&attr);
        pthread_attr_init(&attr);
        CPU_ZERO(&cpui);
        CPU_SET(i % (cpu_cores * 2), &cpui);
        pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpui);
        int a = pthread_create(&threads[i], &attr, &load, NULL);
        if (a != 0)
        {
            printf("Can't create thread %d\n", i);
            break;
        }
    }
    pthread_attr_destroy(&attr);
    
    int tmp = 0;
    integrate(&tmp);

    double result = arr[0];
    i = 1;
    for (i = 1; i < threads_requested; ++i)
    {
        int a = pthread_join(threads[i], NULL);
        if (a != 0)
        {
            printf("Can't join thread %d\n", i);
            break;
        }
        result += arr[i];
    }

    for (i = threads_requested; i < cpu_cores * 2; ++i)
    {
        int a = pthread_cancel(threads[i]);
        if (a != 0)
        {
            printf("Can't cancel thread %d\n", i);
            break;
        }
    }

    free(arr);
    free(threads);
    free(data);

    return result;
}

void* integrate(void* arg)
{
    int number = *((int*)arg);
    double cur = BEGIN + number * diff;
    double end = BEGIN + (number + 1) * diff;
    double val = 0.0;
    while (cur < end)
    {
        val += func(cur) * INTEGRATION_STEP;
        cur += INTEGRATION_STEP;
    }
    arr[number] = val;

    return NULL;
}

void* load(void* arg)
{
    while (1);
}