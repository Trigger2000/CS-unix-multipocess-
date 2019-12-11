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
};

void child(int rd, int wd, int num);
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

    connection* con = (connection*)calloc(n, sizeof(connection));

    pid_t id = 0;
    int i = 0, chtopt[2], pttoch[2];
    for (i = 0; i < n; ++i)
    {
        pipe(chtopt);
        pipe(pttoch);
        id = fork();
        if (id == 0)
        {
            break;
        }
        else
        {
            connection temp;
            temp.chtopt[READ] = chtopt[READ];
            temp.chtopt[WRITE] = chtopt[WRITE];
            temp.pttoch[READ] = pttoch[READ];
            temp.pttoch[WRITE] = pttoch[WRITE];
            con[i] = temp;
        }
    }

    if (id == 0)
    {
        for (int j = 0; j < i; ++j)
        {
            close(con[j].chtopt[READ]);
            close(con[j].chtopt[WRITE]);
            close(con[j].pttoch[READ]);
            close(con[j].pttoch[WRITE]);
        }

        if (i == 0)
        {
            close(chtopt[READ]);
            close(pttoch[WRITE]);
            close(pttoch[READ]);
            child(fd, chtopt[WRITE], i);
        }
        else
        {
            close(chtopt[READ]);
            close(pttoch[WRITE]);
            child(pttoch[READ], chtopt[WRITE], i);
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

void child (int rd, int wd, int num)
{
    char* buf = (char*)calloc(1024, sizeof(char));
    while (1)
    {
        if (strlen(buf) == 0)
        {
            if (read(rd, buf, 10) == 0)
            {
                //printf("child %d read dead\n", num);
                break;
            }
        }

        if (strlen(buf) != 0)
        {
            if (write(wd, buf, strlen(buf)) == -1)
            {
                //printf("child %d write dead\n", num);
                break;
            }
            memset(buf, 0, 1024);
        }
    }
}

void parent (int n, connection* con)
{
    fd_set setr, setw;
    FD_ZERO(&setr);
    FD_ZERO(&setw);
    int max = 0;
    for (int i = 0; i < n; ++i)
    {
        FD_SET(con[i].chtopt[READ], &setr);
        if (con[i].chtopt[READ] > max)
        {
            max = con[i].chtopt[READ];
        }

        if (i != 0)
        {
            FD_SET(con[i].pttoch[WRITE], &setw);
            if (con[i].pttoch[WRITE] > max)
            {
                max = con[i].pttoch[WRITE];
            }
            fcntl(con[i].pttoch[WRITE], F_SETFL, O_NONBLOCK);
        }
        fcntl(con[i].chtopt[READ], F_SETFL, O_NONBLOCK);
    }

    char** bufs = (char**)calloc(n, sizeof(char*));
    for (int i = 0; i < n; ++i)
    {
        bufs[i] = (char*)calloc(4 * 1024, sizeof(char));
        close(con[i].chtopt[WRITE]);
        close(con[i].pttoch[READ]);
    }
    close(con[0].pttoch[WRITE]);

    int in = 0, out = 0, first_child_dead = 0, child_dead = 0; 
    while (1)
    {
        //fd_set setr_copy = setr;
        //fd_set setw_copy = setw;

        //int check = select(max + 1, &setr_copy, &setw_copy, NULL, NULL);
        //printf("check res %d\n", check);
        /*if (check == -1)
        {
            printf("Parent error\n");
            exit(EXIT_FAILURE);
        } */

        for (int i = 0; i < n; ++i)
        {
            fd_set setr_copy = setr;
            fd_set setw_copy = setw;
            //printf("not selected\n");
            int check = select(max + 1, &setr_copy, &setw_copy, NULL, NULL);
            //printf("selected\n");

            //printf("start reading\n");
            int tmp = read(con[i].chtopt[READ], NULL, 1);
            //printf("finish reading\n");
            if (tmp == 0 && i == 0)
            {
                first_child_dead++;
            }

            if (tmp == 0 && i != 0)
            {
                child_dead++;
            }
            //printf("tmp is %d\n", tmp);
            //printf("start op\n");
            if (FD_ISSET(con[i].chtopt[READ], &setr) && strlen(bufs[i]) == 0)
            {
                if (i == 0)
                {
                    in += read(con[i].chtopt[READ], bufs[i], 10);
                }
                else
                {
                    read(con[i].chtopt[READ], bufs[i], 10);
                }
            }

            if (FD_ISSET(con[i + 1].pttoch[WRITE], &setw) && strlen(bufs[i]) != 0 && i < n - 1)
            {
                int amount = write(con[i + 1].pttoch[WRITE], bufs[i], strlen(bufs[i]));
                if (amount > 0)
                {
                    //printf("writing to child %d succeded\n", i + 1);
                }
                memset(bufs[i], 0, 4 * 1024);
            }

            if (strlen(bufs[i]) != 0 && i == n - 1)
            {
                out += write(STDOUT_FILENO, bufs[i], strlen(bufs[i]));
                memset(bufs[i], 0, 4 * 1024);
            }
            //printf("end op\n");
        }
        
        /*printf("in is %d\n", in);
        printf("out is %d\n", out);s
        printf("child dead is %d\n", child_dead);
        printf("first child dead is %d\n", first_child_dead);
        sleep(1); */
        if ((in == out && first_child_dead > 0) || (child_dead > 0 && first_child_dead == 0))
        {
            exit(EXIT_SUCCESS);
        }
    }
}