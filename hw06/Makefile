EXE = main
CC = gcc
CFLAGS = -g -Wall -O2

.PHONY: all clean

all: $(EXE)

$(EXE): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(EXE) *.o core*
