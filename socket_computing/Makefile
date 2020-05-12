.PHONY: all clean

all:
	gcc server.c -o server
	gcc worker.c -o worker
	gcc tmp_server.c -o tmp_server
	gcc tmp_client.c -o tmp_client

clean:
	rm -rf server worker tmp_server tmp_client
