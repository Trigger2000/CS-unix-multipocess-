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
#include <sys/stat.h>
#include <fcntl.h>

const int key = 36;

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
int getsemset();

int main(int argc, char** argv)
{
    int file = open(argv[1], O_RDONLY);
    if (file == -1)
    {
        printf("Unable to open file %s, try again\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int sem = getsemset();

    char buf = 'a';
    while (buf != '\0')
    {
        read(file, &buf, 1);
        //read(file, &buf, 1);
        //print();

        dec(sem, 0);
        dec(sem, 1);
        //print();

        key_t key = ftok("key_file_shm", key);
        int mem = shmget(key, 1, 0777 | IPC_CREAT | IPC_EXCL);
        if (mem == -1)
        {
            mem = shmget(key, 1, 0777);
        }
        char* membuf = (char*)shmat(mem, NULL, 0);
        strcpy(membuf, &buf);
        shmdt(membuf);

        inc(sem, 1);
        inc(sem, 2);
        //print();
    }

    //semctl(sem, 0, IPC_RMID);
    //shmctl(mem, IPC_RMID, NULL);
    return 0;
}

void inc (int sem, int number)
{
    sembuf temp;
    temp.sem_num = number;
    temp.sem_flg = SEM_UNDO;
    temp.sem_op = 1;
    if (semop(sem, &temp, 1) == -1)
    {
        //printf("inc error\n");
    }
}

void dec (int sem, int number)
{
    sembuf temp;
    temp.sem_num = number;
    temp.sem_flg = SEM_UNDO;
    temp.sem_op = -1;
    if (semop(sem, &temp, 1) == -1)
    {
        //printf("dec error\n");
    }
}

int getsemset()
{
    key_t sem_key = ftok("key_file_sem", key);
    int sem = semget(sem_key, 3, 0777 | IPC_CREAT | IPC_EXCL); //0 = empty, 1 = mutex, 2 = full
    //printf("%d\n", sem);
    if (sem == -1)
    {
        //printf("already exist\n");
        sem = semget(sem_key, 3, 0777);
    }
    else
    {
        //printf("created\n");
        union semun temp;
        temp.val = 1;
        semctl(sem, 0, SETVAL, temp);
        semctl(sem, 1, SETVAL, temp);
    }

    return sem;
}