#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "LCD.h"

int main()
{
	// Assign the used LCD pins for the display
	struct LCD disp = {
		.RS = 17,
		.E = 27,
		.D4 = 6,
		.D5 = 13,
		.D6 = 19,
		.D7 = 26
	};

	// Initialize the LCD
	if (LCD_init(&disp)) {
		perror("Error initializing LCD");
		LCD_deinit(&disp);
		return 1;
	}

	return LCD_deinit(&disp);
}

