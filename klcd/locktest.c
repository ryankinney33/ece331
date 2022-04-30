#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

/*
 * The test program for locking.
 * Spawns 2 children.
 * One child opens /dev/lcd nonblocking and the other
 * two do not. The three processes will then try to
 * write to the LCD simultaneously.
 * After all children exit, the parent will reopen the file
 * nonblocking and write to the LCD.
 */
int main()
{
	pid_t child;
	pid_t child1;
	int wstatus;
	int i;
	int cnt;
	char buf[5]; // buffer to be written to the LCD
	int flags = O_WRONLY; // file access flags


	// Spawn some child processes
	child = fork();
	if (child < 0) { // fork error
		perror("fork");
		return 1;
	} else if (child) { // parent
		// spawn another child
		child1 = fork();
		if (child1 < 0) {
			perror("fork");
			return 1;
		} else if (child1) {
			// This is the parent, print PRNT
			strcpy(buf, "PRNT");
		} else {
			// add nonblocking and modify the buffer
			flags |= O_NONBLOCK;
			strcpy(buf, "NBLK"); // print NBLK
		}
	} else { // child; print CHLD
		strcpy(buf, "CHLD");
	}

	// Start by opening the lcd file
	int fd = open("/dev/lcd", flags);
	if (fd < 0) {
		perror("Opening lcd");
		return 1;
	}

	// Write buf to the LCD 4 times in quick succession
	for (i = 0; i < 4; ++i) {
		if ((cnt = write(fd, buf, 4)) < 0) {
			perror("write");
			return 1; // exit if the write failed
		} else {
			printf("%s: %d bytes written.\n",
					(child) ? "Parent" : "Child", cnt);
		}
	}

	if(child) { // Wait for child
		if (waitpid(child, &wstatus, 0) < 1) {
			perror("Error from waitpid");
			return 1;
		}

		// Reopen the file with nonblocking
		close(fd);
		fd = open("/dev/lcd", O_WRONLY | O_NONBLOCK);
		if (fd < 0) {
			perror("open");
			return 1;
		}

		// Write some characters
		// The purpose of this test is to show that nonblocking writes happen
		// if the lock is acquired.
		printf("File opened nonblocking; writing some characters.\n");
		if (write(fd, "Nonblocking", 11) < 0) {
			perror("write");
			return 1;
		}
	}

	// Finally, close the file and exit
	close(fd);
	return 0;
}
