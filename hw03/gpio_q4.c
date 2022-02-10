#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

#include "gpio_q4.h"


int gpio_export(int pin)
{
	/* Open the file to export the GPIO pin */
	int fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) { /* error checking */
		return 1;
	}

	/* Write the pin number to export it and perform error checking */
	if (dprintf(fd, "%d", pin) < 0) {
		close(fd);
		return 1;
	}

	close(fd);
	return 0;
}

int gpio_unexport(int pin)
{
	/* Open the file to unexport the GPIO pin */
	int fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) { /* error checking */
		return 1;
	}

	/* Write the pin number to unexport it and perform error checking */
	if (dprintf(fd, "%d", pin) < 0) {
		close(fd);
		return 1;
	}

	close(fd);
	return 0;
}

int gpio_value(int pin, int value)
{
	char path[50]; /* Holds the path of the file to be opened */

	if (value != 0 && value != 1)
		return 1; /* Sanity check */

	/* Generate the path depending on the passed pin number */
	snprintf(path, 50, "/sys/class/gpio/gpio%d/value", pin);

	/* Open the file and attempt to write the value */
	int fd = open(path, O_WRONLY);
	if (fd < 0) { /* error checking */
		return 1;
	} else if (dprintf(fd, "%d", value) < 0) {
		close(fd);
		return 1;
	}

	close(fd);
	return 0;
}

int gpio_direction(int pin, char *direction)
{
	char path[50]; /* Holds the path of the file to be opened */

	/* Perform some error checking */
	if (direction == NULL || (strcmp(direction, "out") != 0
				&& strcmp(direction, "in") != 0)) {
		return 1;
	}

	/* Generate the path depending on the passed pin number */
	snprintf(path, 50, "/sys/class/gpio/gpio%d/direction", pin);

	/* Open the file and attempt to write the direction */
	int fd = open(path, O_WRONLY);
	if (fd < 0) { /* error checking */
		return 1;
	}
	if (dprintf(fd, "%s", direction) < 0) {
		close(fd);
		return 1;
	}

	close(fd);
	return 0;
}
