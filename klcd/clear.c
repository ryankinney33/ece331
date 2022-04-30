#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>


#include "lcd.h"

/*
 * A test program for ioctl.
 * Opens the /dev/lcd file, writes the LCD
 * clear instruction via ioctl, then closes the file.
 * This program is also useful between tests to clear
 * the screen.
 */
int main()
{
	// Open with error checking
	int fd = open("/dev/lcd", O_WRONLY);
	if (fd < 0) {
		perror("Opening LCD");
		return 1;
	}

	// Clear the display with ioctl
	if (ioctl(fd, LCD_IOC_INSTR, LCD_CLR_DISP) < 0) {
		perror("Clearing LCD");
		return 1;
	}

	close(fd); // Test the close function
	return 0;
}
