# Usage:
# make		Compile all binaries
# make clean	Deleate all binaries

CC=gcc
CFLAGS = -Wall -O3

all: hw1q4 hw1q5

hw1q4.out: hw1q4.o
	$(CC) hw1q4.o -o hw1q4.out

hw1q5.out: hw1q5.o
	$(CC) hw1q5.o -o hw1q5.out

# Rule for making all the object files
%.o: %.c
	$(CC) -c $< -o $@

# Object file prerequisites?
hw1q4.o: hw1q4.c
hw1q5.o: hw1q5.c

clean:
	rm -f *.o *.out
