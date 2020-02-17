TARGET = a.out
CFLAGS = -g -O0 -MD

.PHONY: all clean

all: $(TARGET)

$(TARGET): 1.o 2.o res.o
	gcc *.o -o $(TARGET)

$@: $< header.h
	$(cc) $(CFLAGS) $< -c -o $@

clean:
	rm -rf  $(TARGET) *.o *.d
