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
    char* cur;
    char* end;
    char* begin;
    char* buf;
    int size;
    int dead = 0;
};

void child(int rd, int wd, int num);
void parent(int n, connection* con);
int getsize(int n, int i);

int main(int argc, char** argv)
{
    int n = strtol(argv[1], NULL, 10);
    if (n <= 0 || n > 509)
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
            close(chtopt[WRITE]);
            close(pttoch[READ]);
            temp.chtopt[READ] = chtopt[READ];
            temp.pttoch[WRITE] = pttoch[WRITE];
            con[i] = temp;
        }
    }

    if (id != 0)
    {
        close(con[0].pttoch[WRITE]);
        parent(n, con);
    }
    else
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

    exit(EXIT_SUCCESS);
}

void child (int rd, int wd, int num)
{
    int size = 81 * 1024;
    char* buf = (char*)calloc(size, sizeof(char));
    while (1)
    {
        int amount = read(rd, buf, 1024);
        if (amount == 0)
        {
            break;
        }
        //if (num == 2)
            //sleep(5);
        
        write(wd, buf, amount);
    }
}

void parent (int n, connection* con)
{
    for (int i = 0; i < n; ++i)
    {
        con[i].size = getsize(n, i);
        con[i].size *= 1024;
        con[i].buf = (char*)calloc(con[i].size, sizeof(char));
        con[i].begin = con[i].buf;
        con[i].end = con[i].begin;
        con[i].end += con[i].size;
        con[i].cur = con[i].begin;
        fcntl(con[i].pttoch[WRITE], F_SETFL, O_NONBLOCK);
        fcntl(con[i].chtopt[READ], F_SETFL, O_NONBLOCK);
    }

    int last_dead = -1;
    while (1)
    {
        fd_set setr, setw;
        FD_ZERO(&setr);
        FD_ZERO(&setw);
        int max = 0;
        for (int i = last_dead + 1; i < n; ++i)
        {
            if (con[i].cur != con[i].end) 
            {
                FD_SET(con[i].chtopt[READ], &setr);
                if (con[i].chtopt[READ] > max)
                {
                    max = con[i].chtopt[READ];
                }
            }

            if (i != 0 && con[i - 1].cur != con[i - 1].begin)
            {
                FD_SET(con[i].pttoch[WRITE], &setw);
                if (con[i].pttoch[WRITE] > max)
                {
                    max = con[i].pttoch[WRITE];
                }
            }
        }
        
        if (select(max + 1, &setr, &setw, NULL, NULL) == -1)
        {
            printf("Error\n");
            exit(EXIT_FAILURE);
        }

        for (int i = last_dead + 1; i < n; ++i)
        {
            int flag = 0;
            if (i != 0 && con[i - 1].dead == 1 && con[i - 1].buf == con[i - 1].cur)
            {
                close(con[i].pttoch[WRITE]);
                flag = 1;
            }

            if (FD_ISSET(con[i].chtopt[READ], &setr) && con[i].cur != con[i].end)
            {
                int tmp = read(con[i].chtopt[READ], con[i].cur, con[i].end - con[i].cur);
                if (tmp == 0)
                {
                    con[i].dead = 1;
                    last_dead = i;
                    close(con[i].chtopt[READ]);
                    continue;
                }
                else
                {
                    con[i].cur += tmp;
                }
                
            }
            
            if (i != 0 && FD_ISSET(con[i].pttoch[WRITE], &setw) && flag == 0 && con[i - 1].cur != con[i - 1].begin)
            {
                int tmp = write(con[i].pttoch[WRITE], con[i - 1].begin, con[i - 1].cur - con[i - 1].begin); 
                con[i - 1].begin += tmp;
                if (con[i - 1].begin == con[i - 1].cur)
                {
                    con[i - 1].begin = con[i - 1].buf;
                    con[i - 1].cur = con[i - 1].buf;
                }
            }
        }

        if (con[n - 1].cur != con[n - 1].begin)
        {
            int tmp = write(STDOUT_FILENO, con[n - 1].begin, con[n - 1].cur - con[n - 1].begin);
            con[n - 1].begin += tmp;
            if (con[n - 1].begin == con[n - 1].cur)
            {
                con[n - 1].begin = con[n - 1].buf;
                con[n - 1].cur = con[n - 1].buf;
            }
        }

        if (last_dead == n - 1 && con[n - 1].cur == con[n - 1].buf)
        {
            exit(EXIT_SUCCESS);
        }
    }
}

int getsize(int n, int i)
{
    i = n - i - 1;
    int res = 1;
    for (; i > 0; --i)
    {
        res *= 3;
        if (res > 81)
        {
            return 81;
        }
    }

    return res;
}