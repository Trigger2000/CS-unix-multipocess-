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

    while (1)
    {
        sembuf* op = (sembuf*)calloc(2, sizeof(sembuf));
        op[0].sem_num = 2;
        op[0].sem_flg = 0;
        op[0].sem_op = 0;
        op[1].sem_num = 2;
        op[1].sem_flg = 0;
        op[1].sem_op = 1;
        semop(sem, op, 2);

        if (amount <= 0)
        {
            break;
        }

        bcopy(&amount, mem_size_buf, 1);
        bcopy(buf, membuf, amount);
        amount = read(file, buf, 100);

        op[0].sem_num = 3;
        op[0].sem_flg = 0;
        op[0].sem_op = 1;
        semop(sem, op, 1);
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
    int sem = semget(sem_key, 4, 0777 | IPC_CREAT); // 0 - server, 1 - client, 2 - write, 3 - read

    sembuf* tmp = (sembuf*)calloc(3, sizeof(sembuf));
    tmp[0].sem_num = 0;
    tmp[0].sem_op = 0;
    tmp[0].sem_flg = 0;
    tmp[1].sem_num = 1;
    tmp[1].sem_op = 0;
    tmp[1].sem_flg = 0;
    tmp[2].sem_num = 0;
    tmp[2].sem_op = 1;
    tmp[2].sem_flg = SEM_UNDO;
    semop(sem, tmp, 2);

    return sem;
}