# Usage
# make - Compile all binaries
# make clean - Remove ALL binaries
TARGET = main
OBJS = main.o dictionary.o
CC = gcc
CFLAGS = -Wall -g

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) core*
