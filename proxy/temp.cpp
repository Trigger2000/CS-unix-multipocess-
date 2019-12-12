#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define KILO 1024
#define MAX 100


struct connection
{
	int fin;
	int fout;
	
	char * data;
	int size;
	int begin;
	int end;
};

struct tmp
{
	char buff[MAX];
	int size;
};

int size(int const n, int i)
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
		return 128 * KILO;

	return s * KILO;
}

int main(int argc, char * argv[])
{
	if (argc < 3)
		exit(1);

	int const n = strtol(argv[1], NULL, 10);

	struct connection * buffer = (struct connection *) calloc (n + 1, sizeof(struct connection));
	
	int i = 0;
	int pfdin[2];
	int pfdout[2];
	for (i = 0; i < n; ++i)
	{
		if (pipe(pfdin) == -1)
		{ perror("PIPE"); exit(1); }
		if (pipe(pfdout) == -1)
		{ perror("PIPE"); exit(1); }

		if (fork())
		{
			// parant part
			
			close(pfdin[0]);
			close(pfdout[1]);

			buffer[i].fout = pfdin[1];
			buffer[i + 1].fin = pfdout[0];
			
			fcntl(buffer[i].fout, F_SETFL, O_NONBLOCK);
			fcntl(buffer[i + 1].fin, F_SETFL, O_NONBLOCK);
			
			buffer[i].begin = 0;
			buffer[i].end = 0;
			if (i == 0)
			{
				buffer[i].size = MAX;
				buffer[i].data = (char *) calloc (buffer[i].size, sizeof(char));
			} else 
			{
				buffer[i].size = size(n, i);
				buffer[i].data = (char *) calloc (buffer[i].size, sizeof(char));
			}
		}
		else
		{
			break;
		}		
	}	
	if (i < n) // child part
	{
		close(pfdin[1]);
		close(pfdout[0]);

		struct tmp data;
		while(1)
		{
			data.size = read(pfdin[0], data.buff, MAX);

			if (data.size == 0)
				break;

			write(pfdout[1], &data, data.size);
		}
		exit(0);
	}

	// parent part
	
	int fd = open(argv[2], O_RDONLY);
	if (fd == -1) {perror ("No file"); exit(1); }
//printf("%d\n", fd);
	buffer[0].fin = fd; // parent reads from stdout
	buffer[n].fout = 1;

	fcntl(buffer[0].fin, F_SETFL, O_NONBLOCK);
	fcntl(buffer[n].fout, F_SETFL, O_NONBLOCK);

	buffer[n].size = MAX;
	buffer[n].begin = 0;
	buffer[n].end = 0;
	buffer[n].data = (char *) calloc (buffer[i].size, sizeof(char));

	struct stat st;
	fstat(fd, &st);
	long delta = 0;
	int eof = 0;

//for (i = 0; i <= n; ++i) printf("read from %d, write to %d, size %d\n", buffer[i].fin, buffer[i].fout, buffer[i].size); exit(1);

	do
	{
		fd_set readfd;
		fd_set writefd;
		
		FD_ZERO(&readfd);
		FD_ZERO(&writefd);

		int max = 0;
		for (i = 0; i <= n; ++i)
		{
			// If buffer not full, when I can read to it
			if (buffer[i].size != buffer[i].end)
			{
				if (buffer[i].fin > max)
					max = buffer[i].fin;
				FD_SET(buffer[i].fin, &readfd);
			}
			// If buffer not empty, when I can write from it
			if (buffer[i].end != buffer[i].begin)
			{
				if (buffer[i].fout > max)
					max = buffer[i].fout;
				FD_SET(buffer[i].fout, &writefd);
			}
		}

		if (eof)
			FD_CLR(buffer[0].fin, &readfd);

		if (select(max + 1, &readfd, &writefd, NULL, NULL) == -1)
			perror("Select");

		for (i = 0; i <= n; ++i)
		{
			if (FD_ISSET(buffer[i].fin, &readfd))
			{
				int size = read(buffer[i].fin, buffer[i].data + buffer[i].end, buffer[i].size - buffer[i].end);
				if (i == 0 && size == 0)
					eof = 1;
				if (i == 0)
					delta += size;

				buffer[i].end += size;
			}
			if (FD_ISSET(buffer[i].fout, &writefd))
			{
				int size = write(buffer[i].fout, buffer[i].data + buffer[i].begin, buffer[i].end - buffer[i].begin);
				if (i == n)
					delta -= size;

				buffer[i].begin += size;
				if (buffer[i].begin == buffer[i].end)
				{
					buffer[i].begin = 0;
					buffer[i].end = 0;
				}
			}
		}
	}while (delta != 0);
	
	return 0;
}
