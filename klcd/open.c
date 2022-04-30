#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Test program for to test opening the /dev/lcd file.
 * Tests write only, read only, and read+write opening mode.
 * Only write only should successfully be opened.
 */
int main() {
	// Open write only
	int fd = open("/dev/lcd", O_WRONLY);
	if (fd < 0) {
		// Failed!
		perror("Error opening O_WRONLY");
	} else {
		printf("Successfully opened O_WRONLY.\n");
		close(fd);
	}

	// Open read+write
	fd = open("/dev/lcd", O_RDWR);
	if (fd < 0) {
		// Failed!
		perror("Error opening O_RDWR");
	} else {
		printf("Successfully opened O_RDWR.\n");
		close(fd);
	}

	// Open read only
	fd = open("/dev/lcd", O_RDONLY);
	if (fd < 0) {
		// Failed!
		perror("Error opening O_RDONLY");
	} else {
		printf("Successfully opened O_RDONLY.\n");
		close(fd);
	}
	return 0;
}
