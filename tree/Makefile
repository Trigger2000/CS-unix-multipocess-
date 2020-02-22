TARGET = a.out
CFLAGS = -g -O0 -fprofile-arcs -ftest-coverage

.PHONY: all clean

all: $(TARGET)

$(TARGET): tree.o tests.o
	gcc *.o -fprofile-arcs -ftest-coverage -g -o $(TARGET)

tree.o: tree.c tree.h
	gcc $(CFLAGS) tree.c -c -g -o tree.o

tests.o: tests.c
	gcc $(CFLAGS) tests.c -c -g -o tests.o

clean:
	rm -rf  $(TARGET) *.o *.d *.gcno *.gcda *.gcov
