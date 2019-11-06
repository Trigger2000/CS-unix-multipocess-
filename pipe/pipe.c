#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

int main(int argc, char *argv[])
{
	int pipefd[2];
	pipe(pipefd);

	FILE* f = fopen(argv[1], "r");
	if (f == NULL)
	{
		printf("No file\n");
		exit(EXIT_FAILURE);
	}
	
	fseek(f, 0, SEEK_END);
	int amount = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* buf = (char*)calloc(amount + 5, sizeof(char));

	pid_t temp = fork();

	//printf("%d\n", amount);
	if (temp != 0)
	{
		wait(NULL);
		read (pipefd[0], buf, amount);
		printf("%s\n", buf);
	}

	if (temp == 0)
	{
		//close(pipefd[0]);
		fread(buf, sizeof(char), amount, f);

		write(pipefd[1], buf, amount);
	}

	return 0;
}
