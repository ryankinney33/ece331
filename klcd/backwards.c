#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "lcd.h"

/*
 * A simple test program to test the the cursor
 * position is correctly tracked when the cursor
 * mode is set to move left instead of right.
 */
int main()
{
	char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	int fd = open("/dev/lcd", O_WRONLY);
	if (fd < 0) {
		perror("open");
		return 1;
	}

	// Clear the screen
	if (ioctl(fd, LCD_IOC_INSTR, LCD_CLR_DISP) < 0) {
		perror("Clearing LCD");
		return 1;
	}

	// Set LCD cursor mode to reverse then write the string
	if (ioctl(fd, LCD_IOC_INSTR, LCD_ENTRY_MODE) < 0) {
		perror("Setting direction");
		return 1;
	}
	if (write(fd, str, strlen(str)) < 0) {
		perror("write");
		return 1;
	}

	// Set LCD cursor mode to normal
	if (ioctl(fd, LCD_IOC_INSTR, LCD_ENTRY_MODE | LCD_ENTRY_MODE_ID) < 0) {
		perror("Setting direction");
		return 1;
	}
	close(fd);
	return 0;
}

