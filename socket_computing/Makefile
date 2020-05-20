all: master worker

supervisor: master.c 
	$(CC) $(CFLAGS) -o $@ $^

worker: worker.c
	$(CC) $(CFLAGS) -o $@ $^ -lm -pthread

CFLAGS := -Wall -O2 -std=c99
.PHONY: all clean

clean:
	rm -rf *.o master worker
