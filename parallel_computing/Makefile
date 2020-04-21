TARGET = a.out
CFLAGS = -std=c99 -pthread -O0

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.o calculator.o cpuparser.o
	gcc *.o $(CFLAGS) -o $(TARGET)

$@: $< declarations.h
	$(cc) $(CFLAGS) $< -o $@

clean:
	rm -rf $(TARGET) *.o
