# Usage
# make		Compile all binaries
# make clean	Remove all binaries and object files
TARGET = main
OBJS = main.o LCD.o gpio.o
CC = gcc
CFLAGS = -g -Wall

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) core*
