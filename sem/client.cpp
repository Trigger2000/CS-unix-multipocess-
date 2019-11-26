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

const int KEY = 52;

union semun 
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

int getsemset();

int main(int argc, char** argv)
{
    int sem = getsemset();

    key_t key_num = ftok("key_num", KEY);
    int mem_size = shmget(key_num, 1, 0777 | IPC_CREAT);

    key_t key = ftok("key_file_shm", KEY);
    int mem = shmget(key, 100, 0777 | IPC_CREAT);

    void* membuf = shmat(mem, NULL, 0);
    void* membuf_size = shmat(mem_size, NULL, 0);

    //int check = 0;
    while (1)
    {  
        sembuf op;
        op.sem_num = 3;
        op.sem_flg = IPC_NOWAIT;
        op.sem_op = -1;

        if (semop(sem, &op, 1) != -1)
        {
            int size = 0;
            bcopy(membuf_size, &size, 1);
            write(1, membuf, size);
        }
        else
        {

        }

        op.sem_num = 2;
        op.sem_flg = IPC_NOWAIT;
        op.sem_op = -1;
        semop(sem, &op, 1);
    }

    shmdt(membuf);  
    shmdt(membuf_size);

    return 0;
}

int getsemset()
{
    key_t sem_key = ftok("key_file_sem", KEY);
    int sem = semget(sem_key, 4, 0777 | IPC_CREAT); // 0 - server, 1 - client, 2 - write, 3 - read

    sembuf* tmp = (sembuf*)calloc(2, sizeof(sembuf));
    tmp[0].sem_num = 1;
    tmp[0].sem_op = 0;
    tmp[0].sem_flg = 0;
    tmp[1].sem_num = 0;
    tmp[1].sem_op = 0;
    tmp[1].sem_flg = 0;
    tmp[2].sem_num = 1;
    tmp[2].sem_op = 1;
    tmp[2].sem_flg = SEM_UNDO;
    semop(sem, tmp, 2);

    return sem;
}