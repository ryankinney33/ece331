# Usage
# make		Compile the binary
# make clean	Remove all binaries and object files
TARGET = hw3q4
OBJS = hw3q4.o gpio_q4.o
CC = gcc
CFLAGS = -Wall -g

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) core*
