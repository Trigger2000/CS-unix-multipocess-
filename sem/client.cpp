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

union semun 
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

#define print() \
{ \
    printf("0 is %d\n", semctl(sem, 0, GETVAL)); \
    printf("1 is %d\n", semctl(sem, 1, GETVAL)); \
    printf("2 is %d\n\n", semctl(sem, 2, GETVAL)); \
}

void inc (int sem, int number);
void dec (int sem, int number);

int main(int argc, char** argv)
{
    key_t sem_key = ftok("key_file_sem", 18);
    int sem = semget(sem_key, 3, 0777 | IPC_CREAT | IPC_EXCL); //0 = empty, 1 = mutex, 2 = full
    if (sem == -1)
    {
        sem = semget(sem_key, 3, 0777);
    }
    else
    {
        union semun temp;
        temp.val = 1;
        semctl(sem, 0, SETVAL, temp);
        semctl(sem, 1, SETVAL, temp);
    }
    print();

    dec (sem, 2);
    dec (sem, 1);
    print();

    key_t key = ftok("key_file_shm", 10);
    int mem = shmget(key, 1, 0777 | IPC_CREAT | IPC_EXCL);
    if (mem == -1)
    {
        mem = shmget(key, 1, 0777);
    }
    char* membuf = (char*)shmat(mem, NULL, 0);
    printf("%s\n", membuf);
    shmdt(membuf);
    shmctl(mem, IPC_RMID, NULL);

    inc (sem, 1);
    inc (sem, 0);

    print();
    printf("--------------------------\n");
    //semctl(sem, 0, IPC_RMID);

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