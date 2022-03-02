#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "LCD.h"

int main()
{
	char *buffer = "ECE 331"; // The string to print on the display
	int i;

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

	// Write the buffer to the display on row 0
	for (i = 0; buffer[i]; ++i) {
		if (LCD_character_write(&disp, buffer[i], 0, i)) {
			perror("Writing data to the display");
			LCD_deinit(&disp);
			return 1;
		}
	}

	// Unexport the pins and exit
	return LCD_deinit(&disp);
}

