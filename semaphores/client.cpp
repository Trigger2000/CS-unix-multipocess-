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

const int KEY = 55;

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

    //printf("Started\n");
    key_t key_num = ftok("key_num", KEY);
    int mem_size = shmget(key_num, 1, 0777 | IPC_CREAT);

    key_t key = ftok("key_file_shm", KEY);
    int mem = shmget(key, 100, 0777 | IPC_CREAT);

    void* membuf = shmat(mem, NULL, 0);
    void* membuf_size = shmat(mem_size, NULL, 0);
    
    sembuf* startread = (sembuf*)calloc(3, sizeof(sembuf));
    startread[0].sem_num = 0;
    startread[0].sem_op = -1;
    startread[0].sem_flg = IPC_NOWAIT;
    startread[1].sem_num = 0;
    startread[1].sem_op = 1;
    startread[1].sem_flg = 0;
    startread[2].sem_num = 3;
    startread[2].sem_op = -1;
    startread[2].sem_flg = 0;

    sembuf* endread = (sembuf*)calloc(3, sizeof(sembuf));
    endread[0].sem_num = 0;
    endread[0].sem_op = -1;
    endread[0].sem_flg = IPC_NOWAIT;
    endread[1].sem_num = 0;
    endread[1].sem_op = 1;
    endread[1].sem_flg = 0;
    endread[2].sem_num = 2;
    endread[2].sem_op = -1;
    endread[2].sem_flg = 0;

    while (1)
    {  
        if (semop(sem, startread, 3) == -1)
        {
            break;
        }

        //sleep(5);
        int size = 0;
        bcopy(membuf_size, &size, 1);
        write(1, membuf, size);

        if (semop(sem, endread, 3) == -1)
        {
            break;
        }
    }

    shmdt(membuf);  
    shmdt(membuf_size);

    return 0;
}

int getsemset()
{
    key_t sem_key = ftok("key_file_sem", KEY);
    int sem = semget(sem_key, 5, 0777 | IPC_CREAT); // 0 - server, 1 - client, 2 - write, 3 - read, 4 - lock

    //printf("locked\n");
    sembuf* lock = (sembuf*)calloc(2, sizeof(sembuf));
    lock[0].sem_num = 4;
    lock[0].sem_op = 0;
    lock[0].sem_flg = 0;
    lock[1].sem_num = 1;
    lock[1].sem_op = 0;
    lock[1].sem_flg = 0;
    lock[2].sem_num = 1;
    lock[2].sem_op = 1;
    lock[2].sem_flg = SEM_UNDO;
    semop(sem, lock, 3);

    lock[0].sem_num = 0;
    lock[0].sem_op = -1;
    lock[0].sem_flg = 0;
    lock[1].sem_num = 0;
    lock[1].sem_op = 1;
    lock[1].sem_flg = 0;
    lock[2].sem_num = 4;
    lock[2].sem_op = 1;
    lock[2].sem_flg = SEM_UNDO;
    semop(sem, lock, 3);

    //printf("unlocked\n");
    sembuf* setzero = (sembuf*)calloc(1, sizeof(sembuf));
    setzero[0].sem_num = 2;
    setzero[0].sem_op = -1;
    setzero[0].sem_flg = IPC_NOWAIT;
    semop(sem, setzero, 1);

    setzero[0].sem_num = 3;
    setzero[0].sem_op = -1;
    setzero[0].sem_flg = IPC_NOWAIT;
    semop(sem, setzero, 1);

    return sem;
}