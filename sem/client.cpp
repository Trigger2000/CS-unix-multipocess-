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
#include <sys/shm.h>
#include <sys/sem.h>

const int KEY = 44;

union semun 
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

void inc (int sem, int number);
void dec (int sem, int number);
int getsemset();

int main(int argc, char** argv)
{
    //sleep(15);
    int sem = getsemset();

    if (semctl(sem, 2, GETVAL) == 0)
    {
        exit(EXIT_FAILURE);
    }

    int mem = 0;
    int mem_size = 0;
    while (1)
    {
        if (semctl(sem, 3, GETVAL) == 1)
        {
            shmctl(mem, IPC_RMID, NULL);
            semctl(sem, 0, IPC_RMID);
            break;
        }

        dec (sem, 2);
        dec (sem, 1);

        key_t key_num = ftok("key_num", KEY);
        mem_size = shmget(key_num, 1, 0777 | IPC_CREAT | IPC_EXCL);
        if (mem_size == -1)
        {
            mem_size = shmget(key_num, 1, 0777);
        }

        key_t key = ftok("key_file_shm", KEY);
        mem = shmget(key, 100, 0777 | IPC_CREAT | IPC_EXCL);
        if (mem == -1)
        {
            mem = shmget(key, 100, 0777);
        }

        void* membuf = shmat(mem, NULL, 0);
        void* membuf_size = shmat(mem_size, NULL, 0);
        int size = 0;
        bcopy(membuf_size, &size, 1);
        write(1, membuf, size);
        shmdt(membuf);
        shmdt(membuf_size);

        inc (sem, 1);
        inc (sem, 0);
    }

    return 0;
}

void inc (int sem, int number)
{
    sembuf temp;
    temp.sem_num = number;
    temp.sem_flg = SEM_UNDO;
    temp.sem_op = 1;
    semop(sem, &temp, 1);
}

void dec (int sem, int number)
{
    sembuf temp;
    temp.sem_num = number;
    temp.sem_flg = SEM_UNDO;
    temp.sem_op = -1;
    semop(sem, &temp, 1);
}

int getsemset()
{
    key_t sem_key = ftok("key_file_sem", KEY);
    int sem = semget(sem_key, 4, 0777 | IPC_CREAT | IPC_EXCL); //0 = empty, 1 = mutex, 2 = full, 3 = exit
    if (sem == -1)
    {
        sem = semget(sem_key, 4, 0777);
    }
    else
    {
        union semun temp;
        temp.val = 1;
        semctl(sem, 0, SETVAL, temp);
        semctl(sem, 1, SETVAL, temp);
    }

    return sem;
}