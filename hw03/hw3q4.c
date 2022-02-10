#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "gpio_q4.h"

#define PIN_NUM 17

static int blink = 1;

/* Clears the blink flag to exit the program */
void int_handler()
{
	blink = 0;
}

int main(int argc, char *argv[])
{
	/* Try to trap SIGINT for graceful program exit */
	struct sigaction ctrlc;
	ctrlc.sa_handler = int_handler;
	if (sigaction(SIGINT, &ctrlc, NULL) < 0) {
		fprintf(stderr, "Error from sigaction: %s\n", strerror(errno));
		return 1;
	}

	/* Initialize GPIO17 for output */
	if (gpio_export(PIN_NUM)) {
		fprintf(stderr, "Error exporting GPIO%d: %s\n", PIN_NUM, strerror(errno));
		return 1;
	}

	usleep(1000); /* short delay */

	if (gpio_direction(PIN_NUM, "out")) {
		fprintf(stderr, "Error setting direction: %s\n", strerror(errno));
		gpio_unexport(PIN_NUM);
		return 1;
	}


	while (blink) {
		if (gpio_value(PIN_NUM, 1)) {
			fprintf(stderr, "Error setting value: %s\n", strerror(errno));
			gpio_unexport(PIN_NUM);
			return 1;
		}
		usleep(500000);

		if (gpio_value(PIN_NUM, 0)) {
			fprintf(stderr, "Error setting value: %s\n", strerror(errno));
			gpio_unexport(PIN_NUM);
			return 1;
		}

		usleep(500000);
	}

	gpio_unexport(PIN_NUM);
	printf("\n");

	return 0;
}
