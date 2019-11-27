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
    int file = open(argv[1], O_RDONLY);
    if (file == -1)
    {
        printf("Unable to open file %s, try again\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    void* buf = calloc(100, 1);
    int amount = read(file, buf, 100);

    key_t key_num = ftok("key_num", KEY);
    int mem_size = shmget(key_num, 1, 0777 | IPC_CREAT);

    key_t key = ftok("key_file_shm", KEY);
    int mem = shmget(key, amount, 0777 | IPC_CREAT);

    void* mem_size_buf = shmat(mem_size, NULL, 0);
    void* membuf = shmat(mem, NULL, 0);

    sembuf* startwrite = (sembuf*)calloc(4, sizeof(sembuf));
    startwrite[0].sem_num = 1;
    startwrite[0].sem_op = -1;
    startwrite[0].sem_flg = IPC_NOWAIT;
    startwrite[1].sem_num = 1;
    startwrite[1].sem_op = 1;
    startwrite[1].sem_flg = 0;
    startwrite[2].sem_num = 2;
    startwrite[2].sem_op = 0;
    startwrite[2].sem_flg = 0;
    startwrite[3].sem_num = 2;
    startwrite[3].sem_op = 1;
    startwrite[3].sem_flg = 0;

    sembuf* endwrite = (sembuf*)calloc(3, sizeof(sembuf));
    endwrite[0].sem_num = 1;
    endwrite[0].sem_op = -1;
    endwrite[0].sem_flg = IPC_NOWAIT;
    endwrite[1].sem_num = 1;
    endwrite[1].sem_op = 1;
    endwrite[1].sem_flg = 0;
    endwrite[2].sem_num = 3;
    endwrite[2].sem_op = 1;
    endwrite[2].sem_flg = 0;

    while (amount > 0)
    {
        if (semop(sem, startwrite, 4) == -1)
        {
            break;
        }

        //sleep(5);
        bcopy(&amount, mem_size_buf, 1);
        bcopy(buf, membuf, amount);
        amount = read(file, buf, 100);

        if (semop(sem, endwrite, 3) == -1)
        {
            break;
        }
    }

    shmdt(membuf);
    shmdt(mem_size_buf);

    shmctl(mem, IPC_RMID, NULL);
    shmctl(mem_size, IPC_RMID, NULL);
    semctl(sem, 0, IPC_RMID);

    return 0;
}

int getsemset()
{
    key_t sem_key = ftok("key_file_sem", KEY);
    int sem = semget(sem_key, 5, 0777 | IPC_CREAT); // 0 - server, 1 - client, 2 - write, 3 - read, 4 - lock

    //printf("locked\n");
    sembuf* lock = (sembuf*)calloc(3, sizeof(sembuf));
    lock[0].sem_num = 4;
    lock[0].sem_op = 0;
    lock[0].sem_flg = 0;
    lock[1].sem_num = 0;
    lock[1].sem_op = 0;
    lock[1].sem_flg = 0;
    lock[2].sem_num = 0;
    lock[2].sem_op = 1;
    lock[2].sem_flg = SEM_UNDO;
    semop(sem, lock, 3);

    lock[0].sem_num = 1;
    lock[0].sem_op = -1;
    lock[0].sem_flg = 0;
    lock[1].sem_num = 1;
    lock[1].sem_op = 1;
    lock[1].sem_flg = 0;
    lock[2].sem_num = 4;
    lock[2].sem_op = 1;
    lock[2].sem_flg = SEM_UNDO;
    semop(sem, lock, 3);

    //printf("unlocked\n");
    sembuf* waitforzero = (sembuf*)calloc(4, sizeof(sembuf));
    waitforzero[0].sem_num = 1;
    waitforzero[0].sem_op = -1;
    waitforzero[0].sem_flg = IPC_NOWAIT;
    waitforzero[1].sem_num = 1;
    waitforzero[1].sem_op = 1;
    waitforzero[1].sem_flg = 0;
    waitforzero[2].sem_num = 2;
    waitforzero[2].sem_op = 0;
    waitforzero[2].sem_flg = 0;
    waitforzero[3].sem_num = 3;
    waitforzero[3].sem_op = 0;
    waitforzero[3].sem_flg = 0;
    if (semop(sem, waitforzero, 4) == -1)
    {
        exit(EXIT_FAILURE);
    }

    return sem;
}