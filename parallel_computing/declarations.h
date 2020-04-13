#pragma once

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>

#define func(x) ((x)*(x))
#define INTEGRATION_STEP 0.000001
#define BEGIN 0
#define END 10000

struct thread_info
{
    double begin;
    double end;
    int num;
};
typedef struct thread_info thread_info;

int getcpuinfo();
double calculate(int cpu_cores, int threads_requested);
void* integrate(void* arg);