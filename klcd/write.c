#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include "lcd.h"

/*
 * A test program to open, write, and close the /dev/lcd file.
 * This program checks that a single process can write to the
 * lcd. Also helps to check that the driver correctly tracks
 * where the cursor is to keep written characters on the screen.
 * In addition, it checks that the driver reads at most 32
 * characters and does not dereference NULL when a NULL pointer
 * is passed to write.
 */
int main()
{
	char buf[100];
	int res;
	int i;

	// Open write only, blocking
	int fd = open("/dev/lcd", O_WRONLY);
	if (fd < 0) {
		perror("Opening lcd");
		return 1;
	}

	// Start by clearing the screen
	// Determines if writing keeps track of the cursor location
	// after an instruction changes it; this also resets cursor direction.
	if (ioctl(fd, LCD_IOC_INSTR, LCD_CLR_DISP) < 0) {
		perror("Clearing display");
		return 1;
	}

	// Try to write 5 characters twice
	strcpy(buf, "Ayaya");
	for (i = 0; i < 2; ++i) {
		res = write(fd, buf, strlen(buf)); // should be 5
		if (res < 0) {
			perror("write");
			return 1;
		} else if (res != 5) {
			fprintf(stderr, "Wrote %d characters; expected 5\n", res);
		} else {
			printf("Wrote %d characters.\n", res);
		}
		sleep(1);
	}

	// Make sure the second line is written to and line wrapping is handled
	// Try to write 20 characters
	memset(buf, '/', 20);
	buf[20] = 0;
	res = write(fd, buf, strlen(buf)); // should be 20
	if (res < 0) {
		perror("write");
		return 1;
	} else if (res != 20) {
		fprintf(stderr, "Wrote %d characters; expected 20\n", res);
	} else {
		printf("Wrote %d characters.\n", res);
	}
	sleep(1);

	// Try to write more than 40 characters to the screen
	// This test also ensures that the second line can wrap.
	memset(buf, 'A', 60);
	buf[60] = 0; // null terminate the string
	res = write(fd, buf, strlen(buf)); // should be 40
	printf("Attempting to write 60 characters:\n");
	if (res < 0) {
		perror("write");
		return 1;
	}
	printf("Wrote %d characters.\n", res);

	// Attempt to write 0 characters
	res = write(fd, buf, 0);
	if (res < 0) {
		perror("write");
		return 1;
	}
	printf("Wrote %d characters.\n", res);

	// Attempt to write a NULL buffer
	printf("Attempting to write characters from NULL:\n");
	res = write(fd, NULL, 300);
	if (res >= 0) {
		fprintf(stderr, "Task was successful???\n");
		return 1;
	}
	perror("write");

	close(fd);
	return 0;
}
