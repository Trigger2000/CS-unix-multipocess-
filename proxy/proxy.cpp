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
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <math.h>

#define READ 0
#define WRITE 1

struct connection
{
    int chtopt[2], pttoch[2];
    char* buf;
};

void child(int rd, int wd);
void parent(int n, connection* con);

int main(int argc, char** argv)
{
    printf("ppid %d\n", getpid());
    int n = strtol(argv[1], NULL, 10);
    if (n <= 0)
    {
        printf("Incorrect amount of children\n");
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[2], O_RDONLY);
    if (fd == -1)
    {
        printf("Unable to open file %s", argv[2]);
        exit(EXIT_FAILURE);
    }

    connection* con = (connection*)calloc(n - 1, sizeof(connection));

    for (int i = 0; i < n - 1; ++i)
    {
        pipe(con[i].chtopt);
        pipe(con[i].pttoch);
    }

    pid_t id = 0;
    int i = 0;
    for (i = 0; i < n; ++i)
    {
        id = fork();
        if (id == 0)
            break;
    }

    if (id == 0)
    {
        if (i == 0)
        {
            close(con[i].chtopt[READ]);
            child(fd, con[i].chtopt[WRITE]);
        }
        else if (i == n - 1)
        {
            close(con[i - 1].pttoch[WRITE]);
            child(con[i - 1].pttoch[READ], STDOUT_FILENO);
        }
        else
        {
            close(con[i].chtopt[READ]);
            close(con[i - 1].pttoch[WRITE]);
            child(con[i - 1].pttoch[READ], con[i].chtopt[WRITE]);
        }
    }
    else
    {
        if (n == 1)
        {
            exit(EXIT_SUCCESS);
        }

        parent(n, con);
    }

    exit(EXIT_SUCCESS);
}

void child (int rd, int wd)
{
    char* buf = (char*)calloc(1024, sizeof(char));
    while (1)
    {
        if (strlen(buf) != 0)
        {
            if (write(wd, buf, strlen(buf)) == -1)
            {
                break;
            }
            memset(buf, 0, 1024);
        }

        if (strlen(buf) == 0)
        {
            if (read(rd, buf, 10) == 0)
            {
                break;
            }
        }
    }
}

void parent (int n, connection* con)
{
    char** bufs = (char**)calloc(n - 1, sizeof(char*));
    for (int i = 0; i < n - 1; ++i)
    {
        bufs[i] = (char*)calloc(4 * 1024, sizeof(char));
        close(con[i].chtopt[WRITE]);
        close(con[i].pttoch[READ]);
    }

    fd_set setr, setw;
    FD_ZERO(&setr);
    FD_ZERO(&setw);
    int max = 0;
    for (int i = 0; i < n - 1; ++i)
    {
        FD_SET(con[i].chtopt[READ], &setr);
        if (con[i].chtopt[READ] > max)
        {
            max = con[i].chtopt[READ];
        }

        FD_SET(con[i].pttoch[WRITE], &setw);
        if (con[i].pttoch[WRITE] > max)
        {
            max = con[i].pttoch[WRITE];
        }
        fcntl(con[i].chtopt[READ], F_SETFL, O_NONBLOCK);
        fcntl(con[i].pttoch[WRITE], F_SETFL, O_NONBLOCK);
    }

    int in = -1, out = -2, flag1 = 0, flag2 = 0, first_child_dead = 0, child_dead = 0; 
    while (1) //рассмотри при маленьком n
    {
        fd_set setr_copy = setr;
        fd_set setw_copy = setw;

        int check = select(max + 1, &setr_copy, &setw_copy, NULL, NULL);
        if (check == -1)
        {
            printf("Parent error\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n - 1; ++i)
        {
            int tmp = read(con[i].chtopt[READ], NULL, 1);
            if (tmp == -1 && i == 0)
            {
                first_child_dead++;
            }
            else if (tmp == -1)
            {
                child_dead++;
            }

            if (i == n - 2)
            {
                write(con[i].pttoch[WRITE], NULL, 1);
            }

            if (FD_ISSET(con[i].chtopt[READ], &setr) && strlen(bufs[i]) == 0)
            {
                if (i == 0)
                {
                    if (flag1 == 0)
                    {
                        in = 0;
                        flag1++;
                    }
                    int tmp = read(con[i].chtopt[READ], bufs[i], 10);
                    if (tmp != -1)
                        in += tmp;
                }
                else
                {
                    read(con[i].chtopt[READ], bufs[i], 10);
                }
            }

            if (FD_ISSET(con[i].pttoch[WRITE], &setw) && strlen(bufs[i]) != 0)
            {
                if (i == n - 2)
                {
                    if (flag2 == 0)
                    {
                        out = 0;
                        flag2++;
                    }
                    int tmp = write(con[i].pttoch[WRITE], bufs[i], strlen(bufs[i]));
                    if (tmp != -1)
                        out += tmp;
                }
                else
                {
                    write(con[i].pttoch[WRITE], bufs[i], strlen(bufs[i]));
                }
                memset(bufs[i], 0, 4 * 1024);
            }
        }
        
        /*printf("in is %d\n", in);
        printf("out is %d\n", out);
        printf("child dead is %d\n", child_dead);
        printf("first child dead is %d\n", first_child_dead); */
        //sleep(1);
        if ((in == out && first_child_dead > 0) || (child_dead > 0 && first_child_dead == 0))
        {
            exit(EXIT_SUCCESS);
        }
    }
}