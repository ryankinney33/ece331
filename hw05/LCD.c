#include <unistd.h>

#include "gpio.h"
#include "LCD.h"

/* Exports and sets the required GPIO pins as outputs */
static int LCD_gpio_init()
{
	int result;

	// Export the required pins
	result = gpio_export(LCD_RS);
	result |= gpio_export(LCD_E);
	result |= gpio_export(DATA_D4);
	result |= gpio_export(DATA_D5);
	result |= gpio_export(DATA_D6);
	result |= gpio_export(DATA_D7);
	if (result) // Error Checking
		return 1;

	// Set the pins as output
	result = gpio_direction(LCD_RS, "out");
	result |= gpio_direction(LCD_E, "out");
	result |= gpio_direction(DATA_D4, "out");
	result |= gpio_direction(DATA_D5, "out");
	result |= gpio_direction(DATA_D6, "out");
	result |= gpio_direction(DATA_D7, "out");
	if (result) // Error Checking
		return 1;

	return 0; // No problems encountered.
}

/* Initialize the LCD */
int LCD_init()
{
	// The required GPIO pins need to be configured
	if (LCD_gpio_init()) {
		return 1;
	}
	usleep(15010); // Delay for more than 15 ms after Vcc = 4.5V
	// Send Function Set 8 command
	if (LCD_instruction_write(LCD_FCNT_SET |
			LCD_FNCT_SET_DL | LCD_FNCT_INIT)) {
		return 1;
	}
	usleep(4110); // Delay for more than 4.1 ms
	// Send another Function Set 8 command
	if (LCD_instruction_write(LCD_FCNT_SET |
			LCD_FNCT_SET_DL | LCD_FNCT_INIT)) {
		return 1;
	}
	usleep(101); // Delay for more than 100 microseconds
	// Send the final Function Set 8 command
	if (LCD_instruction_write(LCD_FCNT_SET |
			LCD_FNCT_SET_DL | LCD_FNCT_INIT)) {
		return 1;
	}
	usleep(41);
	// Send a Function Set 4 command
	if (LCD_instruction_write(LCD_FCNT_SET | LCD_FNCT_INIT))
		return 1;
	usleep(41);
	// Configure the display
	if (LCD_instruction_write(LCD_FCNT_SET | LCD_FNCT_SET_N)) // 2 line; 5x7
		return 1;
	usleep(41);
	// Turn the display off
	if (LCD_instruction_write(LCD_ON_OFF_CTRL))
		return 1;
	usleep(41);
	// Clear the display
	if (LCD_instruction_write(LCD_CLR_DISP))
		return 1;
	usleep(15210);
	// Set the entry mode to increment without display shift
	if (LCD_instruction_write(LCD_ON_OFF_CTRL | LCD_ON_OFF_CTRL_D |
			LCD_ON_OFF_CTRL_C | LCD_ON_OFF_CTRL_B)) {
		return 1;
	}
	usleep(41);
	return 0; // Initialization done;
}

/* Write data to the LCD */
int LCD_data_write();

/* Write an instruction to the LCD */
int LCD_instruction_write(char instruction);

/* Write a character to a specific location on the display */
int LCD_character_write();
