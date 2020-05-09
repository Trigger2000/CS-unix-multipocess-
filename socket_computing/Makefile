TARGET1 = server
TARGET2 = worker

.PHONY: all clean

all: $(TARGET1) $(TARGET2)

$(TARGET1): server.o
	gcc server.o -o $(TARGET1)

$(TARGET2): worker.o
	gcc worker.o -o $(TARGET2)

$@: $<
	$(cc) $< -c -o $@

clean:
	rm -rf $(TARGET1) $(TARGET2) *.o