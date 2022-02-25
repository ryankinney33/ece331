#include <unistd.h>

#include "gpio.h"
#include "LCD.h"

/* Exports and sets the required GPIO pins as outputs */
static int LCD_gpio_init(const struct LCD *disp)
{
	int r;

	// Export the required pins
	r = gpio_export(disp->RS);
	r |= gpio_export(disp->E);
	r |= gpio_export(disp->D4);
	r |= gpio_export(disp->D5);
	r |= gpio_export(disp->D6);
	r |= gpio_export(disp->D7);
	if (r) // Error Checking
		return 1;

	// Set the pins as output
	r = gpio_direction(disp->RS, "out");
	r |= gpio_direction(disp->E, "out");
	r |= gpio_direction(disp->D4, "out");
	r |= gpio_direction(disp->D5, "out");
	r |= gpio_direction(disp->D6, "out");
	r |= gpio_direction(disp->D7, "out");
	if (r) // Error Checking
		return 1;

	return 0; // No problems encountered.
}

/*
 * Sets the RS and data GPIO then sets the E pin to write to the display.
 * The bits of val represent RS,D7,D6,D5,D4
 * t is the delay to wait after writing the instruction
 */
static int LCD_pin_set(const struct LCD *disp, char val, unsigned int t)
{
	int r;

	// Decode val and set pins accordingly
	r = gpio_value(disp->RS, (val >> 4) & 1);
	r |= gpio_value(disp->D7, (val >> 3) & 1);
	r |= gpio_value(disp->D6, (val >> 2) & 1);
	r |= gpio_value(disp->D5, (val >> 1) & 1);
	r |= gpio_value(disp->D4, val & 1);
	if (r)
		return 1; // Error checking

	// Pull the E pin high to send the instruction
	if (gpio_value(disp->E, 1))
		return 1;
	usleep(t);

	return gpio_value(disp->E, 0); // Pull E low for the next instruction
}

/* Initialize the LCD */
int LCD_init(const struct LCD *disp)
{
	// The required GPIO pins need to be configured
	if (LCD_gpio_init(disp)) {
		return 1;
	}
	usleep(15010); // Delay for more than 15 ms after Vcc = 4.5V

	// Send Function Set 8 command
	if (LCD_instruction_write(disp, LCD_FNCT_SET |
			LCD_FNCT_SET_DL | LCD_FNCT_INIT, 4100)) {
		return 1;
	}

	// Send another Function Set 8 command
	if (LCD_instruction_write(disp, LCD_FNCT_SET |
			LCD_FNCT_SET_DL | LCD_FNCT_INIT, 100)) {
		return 1;
	}

	// Send the final Function Set 8 command
	if (LCD_instruction_write(disp, LCD_FNCT_SET |
			LCD_FNCT_SET_DL | LCD_FNCT_INIT, 40)) {
		return 1;
	}

	// Send a Function Set 4 command
	if (LCD_instruction_write(disp, LCD_FNCT_SET | LCD_FNCT_INIT, 40))
		return 1;

	// Configure the display; 2 lines, 5x7 font
	if (LCD_instruction_write(disp, LCD_FNCT_SET | LCD_FNCT_SET_N, 40))
		return 1;

	// Turn the display off
	if (LCD_instruction_write(disp, LCD_ON_OFF_CTRL, 40))
		return 1;

	// Clear the display
	if (LCD_instruction_write(disp, LCD_CLR_DISP, 15200))
		return 1;

	// Set the entry mode to increment without display shift
	if (LCD_instruction_write(disp, LCD_ON_OFF_CTRL | LCD_ON_OFF_CTRL_D |
			LCD_ON_OFF_CTRL_C | LCD_ON_OFF_CTRL_B, 41)) {
		return 1;
	}

	return 0; // Initialization done;
}

/* Write data to the LCD */
int LCD_data_write(const struct LCD *disp);

/* Write an instruction to the LCD */
int LCD_instruction_write(const struct LCD *disp, char instr, unsigned int t)
{
	char data = (instr & 0xF0) >> 4; // Get the first part of instr

	// Check for the FNCT_SET_INIT instructions, which act differently
	if ((data == (LCD_FNCT_SET >> 4)) &&
			((instr & 0x03) == LCD_FNCT_INIT)) {
		// Write the instruction to the LCD
		return LCD_pin_set(disp, data, t);
	}

	// Normal instruction writing
	// Write the first half
	if (LCD_pin_set(disp, data, 2))
		return 1;

	// Write the second half
	return LCD_pin_set(disp, data, t);
}

/* Write a character to a specific location on the display */
int LCD_character_write();
