# Usage:
# make		Compile all binaries
# make clean	Deleate all binaries

CC=gcc
CFLAGS = -Wall -g -O3

all: hw2q7.out hw2q8.out

hw2q7.out: hw2q7.o
	$(CC) hw2q7.o -o hw2q7.out

hw2q8.out: hw2q8.o
	$(CC) hw2q8.o -o hw2q8.out

# Rule for making all the object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Object file prerequisites?
hw2q7.o: hw2q7.c
hw2q8.o: hw2q8.c

clean:
	rm -f *.o *.out
