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
    int filled = 0;
    int size = 0;
};

void child(int rd, int wd, int num);
void parent(int n, connection* con);
int getsize(int n, int i);

int main(int argc, char** argv)
{
    int n = strtol(argv[1], NULL, 10);
    if (n <= 0 || n > 255)
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
        parent(n, con);
    }

    exit(EXIT_SUCCESS);
}

void child (int rd, int wd, int num)
{
    char* buf = (char*)calloc(4096, sizeof(char));
    while (1)
    {
        int amount = read(rd, buf, 4096);
        if (amount == 0)
        {
            break;
        }
        
        write(wd, buf, amount);
        memset(buf, 0, 4096);
    }
}

void parent (int n, connection* con) //исправь расположение в массивах
{
    char** bufs = (char**)calloc(n, sizeof(char*));
    for (int i = 0; i < n; ++i)
    {
        con[i].size = getsize(n, i);
        bufs[i] = (char*)calloc(1024, sizeof(char));
        close(con[i].chtopt[WRITE]);
        close(con[i].pttoch[READ]);
        fcntl(con[i].pttoch[WRITE], F_SETFL, O_NONBLOCK);
        fcntl(con[i].chtopt[READ], F_SETFL, O_NONBLOCK);
    }
    close(con[0].pttoch[WRITE]);

    int in = 0, out = 0, first_child_dead = 0, child_dead = 0; 

    while (1)
    {
        fd_set setr, setw;
        FD_ZERO(&setr);
        FD_ZERO(&setw);
        int max = 0;
        for (int i = 0; i < n; ++i)
        {
            if (strlen(bufs[i]) == 0) 
            {
                FD_SET(con[i].chtopt[READ], &setr);
                if (con[i].chtopt[READ] > max)
                {
                    max = con[i].chtopt[READ];
                }
            }

            if (i != 0 && strlen(bufs[i - 1]) != 0)
            {
                FD_SET(con[i].pttoch[WRITE], &setw);
                if (con[i].pttoch[WRITE] > max)
                {
                    max = con[i].pttoch[WRITE];
                }
            }
        }

        if (first_child_dead)
        {
            FD_CLR(con[0].chtopt[READ], &setr);
        }
        
        //printf("not selected\n");
        if (select(max + 1, &setr, &setw, NULL, NULL) == -1)
        {
            printf("select error\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; ++i)
        {
            int check = read(con[i].chtopt[READ], NULL, 1);
            if (check == 0 && i == 0)
            {
                first_child_dead = 1;
            }

            if (check == 0 && i != 0)
            {
                child_dead++;
            }

            if (FD_ISSET(con[i].chtopt[READ], &setr) && con[i].filled == 0)
            {
                if (i == 0)
                {
                    int tmp = read(con[i].chtopt[READ], bufs[i], 10);
                    in += tmp;
                    con[i].filled = tmp;
                }
                else
                {
                    con[i].filled = read(con[i].chtopt[READ], bufs[i], 10);
                }
            }
            
            if (i < n - 1)
            {
                if (FD_ISSET(con[i + 1].pttoch[WRITE], &setw) && con[i].filled != 0)
                {
                    con[i].filled -= write(con[i + 1].pttoch[WRITE], bufs[i], con[i].filled);
                    memset(bufs[i], 0, 1024);
                }
            }

            if (con[i].filled != 0 && i == n - 1)
            {
                int tmp = write(STDOUT_FILENO, bufs[i], con[i].filled);
                out += tmp;
                con[i].filled -= tmp;
                memset(bufs[i], 0, 1024);
            }
            //printf("fiiled is%d\n", con[i].filled);
        }
        
        //printf("\nin is %d\nout is%d\n", in, out);
        //sleep(1);
        if ((in == out && first_child_dead == 1) || child_dead > 0)
        {
            exit(EXIT_SUCCESS);
        }
    }
}

int getsize(int n, int i)
{
    i = n - i - 1;
	int s = 1;
	while (i > 0)
	{
		s *= 3;
		--i;
		if (s > 128)
			break;
	}
	if (s > 128)
		return 81;

	return s;
}