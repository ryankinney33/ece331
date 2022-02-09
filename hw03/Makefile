# Usage
# make		Compile the binary
# make clean	Remove all binaries and object files
TARGET = hw1q5
OBJS = hw1q5.o
CC = gcc
CFLAGS = -Wall -g

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) core*
