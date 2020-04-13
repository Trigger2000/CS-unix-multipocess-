#include "declarations.h"

double* arr;

double calculate(int cpu_cores, int threads_requested)
{
    cpu_set_t cpu0;
    CPU_ZERO(&cpu0);
    CPU_SET(0, &cpu0);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu0);

    double diff = (END - BEGIN) / threads_requested;
    double cur = diff;
    arr = (double*)calloc(threads_requested, 64);
    pthread_t* threads = (pthread_t*)calloc(threads_requested, sizeof(pthread_t));
    thread_info* info = (thread_info*)calloc(threads_requested, 64);

    for (int i = 1; i < threads_requested; ++i)
    {
        cpu_set_t* cpui = (cpu_set_t*)calloc(1, sizeof(cpu_set_t));
        CPU_ZERO(cpui);
        CPU_SET(i % cpu_cores, cpui);
        pthread_attr_t* attr = (pthread_attr_t*)calloc(1, sizeof(cpu_set_t));
        pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), cpui);

        info[i].begin = cur;
        info[i].end = cur + diff;
        info[i].num = i;
        pthread_create(&threads[i], attr, &integrate, &info[i]);
        cur += diff;
    }
    thread_info first = {BEGIN, diff, 0};
    integrate(&first);

    double result = arr[0];
    for (int i = 1; i < threads_requested; ++i)
    {
        pthread_join(threads[i], NULL);
        result += arr[i];
    }
    free(arr);
    free(threads);

    return result;
}

void* integrate(void* arg)
{
    thread_info* tmp = (thread_info*)arg;
    double cur = tmp->begin;
    double val = 0.0;
    while (cur < tmp->end)
    {
        val += func(cur) * INTEGRATION_STEP;
        cur += INTEGRATION_STEP;
    }
    arr[tmp->num] = val;

    return NULL;
}