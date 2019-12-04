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

int getn(int argc, char** argv);
void child(int chnum, int n, connection con);
void parent(int n, connection* con);
void parent_transfer(int n, connection* con, fd_set* setw_copy, fd_set* setr_copy, char** bufs);
void child_transfer(connection* con, fd_set* setw_copy, fd_set* setr_copy);

int main(int argc, char** argv)
{
    int n = getn(argc, argv);

    connection* con = (connection*)calloc(n, sizeof(connection));
    con[0].pttoch[READ] = STDIN_FILENO;
    pipe(con[0].chtopt);
    fcntl(con[0].chtopt[READ], F_SETFD, O_NONBLOCK);

    con[n - 1].chtopt[WRITE] = STDOUT_FILENO;
    pipe(con[n - 1].pttoch);
    fcntl(con[0].pttoch[WRITE], F_SETFD, O_NONBLOCK);

    for (int i = 1; i < n - 1; ++i)
    {
        pipe(con[i].chtopt);
        pipe(con[i].pttoch);
        fcntl(con[i].chtopt[READ], F_SETFD, O_NONBLOCK);
        fcntl(con[i].pttoch[WRITE], F_SETFD, O_NONBLOCK);
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
        child(i, n, con[i]);
    }
    else
    {
        parent(n, con);
    }

    exit(EXIT_SUCCESS);
}

void child (int chnum, int n, connection con)
{
    int temp = pow(3, n - chnum);
    if (temp > 81)
    {
        con.buf = (char*)calloc(81 * 1024, sizeof(char));
    }
    else
    {
        con.buf = (char*)calloc(temp * 1024, sizeof(char));
    }
    
    close(con.chtopt[READ]);
    close(con.pttoch[WRITE]);
    int max = 0;
    if (con.chtopt[WRITE] > con.pttoch[READ])
    {
        max = con.chtopt[WRITE];
    }
    else
    {
        max = con.pttoch[READ];
    }
    
    fd_set setw, setr, setw_copy, setr_copy;
    FD_ZERO(&setw);
    FD_ZERO(&setr);
    FD_SET(con.chtopt[WRITE], &setw);
    FD_SET(con.pttoch[READ], &setr);

    while (1)
    {
        setw_copy = setw;
        setr_copy = setr;
        int check = select(max + 1, &setr_copy, &setw_copy, NULL, NULL);
        if (check == -1)
        {
            printf("child error\n");
            break;
        }
        else
        {
            child_transfer(&con, &setw_copy, &setr_copy);
        }
    }
}

void child_transfer(connection* con, fd_set* setw_copy, fd_set* setr_copy)
{
    if (FD_ISSET(con->pttoch[READ], setr_copy) && strlen(con->buf) == 0)
    {
        read(con->pttoch[READ], con->buf, 1024);
    }

    if (FD_ISSET(con->chtopt[WRITE], setw_copy))
    {
        write(con->chtopt[WRITE], con->buf, 1024);
        memset(con->buf, 0, 1024);
    }
}

void parent (int n, connection* con)
{
    char** bufs = (char**)calloc(n - 1, sizeof(char*));
    for (int i = 0; i < n - 1; ++i)
    {
        int temp = pow(3, n - i);
        if (temp > 81)
        {
            bufs[i] = (char*)calloc(81 * 1024, sizeof(char));
        }
        else
        {
            bufs[i] = (char*)calloc(temp * 1024, sizeof(char));
        }
    }

    fd_set setw, setr, setw_copy, setr_copy;
    FD_ZERO(&setw);
    FD_ZERO(&setr);
    int max = 0;
    for (int i = 0; i < n; ++i)
    {
        close(con[i].chtopt[WRITE]);
        if (i != n - 1)
        {
            FD_SET(con[i].chtopt[READ], &setr);
            if (con[i].chtopt[READ] > max)
            {
                max = con[i].chtopt[READ];
            }
        }

        close(con[i].pttoch[READ]);
        if (i != 0)
        {
            FD_SET(con[i].pttoch[WRITE], &setw);
            if (con[i].pttoch[WRITE] > max)
            {
                max = con[i].pttoch[WRITE];
            }
        }
    }

    while (1)
    {
        setw_copy = setw;
        setr_copy = setr;

        int check = select(max + 1, &setr_copy, &setw_copy, NULL, NULL);
        if (check == -1)
        {
            printf("parent error\n");
            break;
        }
        else
        {
            parent_transfer(n, con, &setw_copy, &setr_copy, bufs);
        }
    }
}

void parent_transfer(int n, connection* con, fd_set* setw_copy, fd_set* setr_copy, char** bufs)
{
    for (int i = 0; i < n; ++i)
    {
        if (i != n - 1)
        {
            if (FD_ISSET(con[i].chtopt[READ], setr_copy) && strlen(bufs[i]) == 0)
            {
                read(con[i].chtopt[READ], bufs[i], 1024);
            }
        }

        if (i != 0)
        {
            if (FD_ISSET(con[i].pttoch[WRITE], setw_copy))
            {
                write(con[i].pttoch[WRITE], bufs[i - 1], 1024);
                memset(bufs[i - 1], 0, 1024);
            }
        }
    } 
}

int getn (int argc, char** argv)
{
    int base;
    char *endptr, *str;
    long val;

    if (argc < 2) 
    {
       fprintf(stderr, "Usage: %s str [base]\n", argv[0]);
       exit(EXIT_FAILURE);
    }

    str = argv[1];
    base = (argc > 2) ? atoi(argv[2]) : 10;

    errno = 0;   
    val = strtol(str, &endptr, base);

    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) 
    {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == str) 
    {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }

    if (*endptr != '\0')
    {
        printf("Further characters after number: %s\n", endptr);
        exit(EXIT_FAILURE);
    }

    return val;
}