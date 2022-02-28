#include <unistd.h>

#include "gpio.h"
#include "LCD.h"

/* Exports and sets the required GPIO pins as outputs */
static int LCD_gpio_init(const struct LCD *disp)
{
	// Export the required pins and perform error checking
	if (gpio_export(disp->RS) ||
			gpio_export(disp->E) || gpio_export(disp->D4) ||
			gpio_export(disp->D5) || gpio_export(disp->D6) ||
			gpio_export(disp->D7)) {
		return 1;
	}

	// Set the pins as output with error checking
	return (gpio_direction(disp->RS, "out") ||
			gpio_direction(disp->E, "out") ||
			gpio_direction(disp->D4, "out") ||
			gpio_direction(disp->D5, "out") ||
			gpio_direction(disp->D6, "out") ||
			gpio_direction(disp->D7, "out"));
}

/* Clocks the LCD display to write the data */
static int LCD_clock(const struct LCD *disp, unsigned int t)
{
	// Pull the E pin high to send the instruction
	if (gpio_value(disp->E, 1))
		return 1;
	usleep(t); // Wait the required time for the instruction execution

	return gpio_value(disp->E, 0); // Pull E low for the next instruction
}


/*
 * Sets the RS and data GPIO then sets the E pin to write to the display.
 * The bits of val represent RS,D7,D6,D5,D4
 * t is the delay to wait after writing the instruction
 */
static int LCD_write(const struct LCD *disp, char val, unsigned int t)
{
	// Decode val and set pins accordingly with error checking
	if (gpio_value(disp->RS, (val >> 4) & 1) ||
			gpio_value(disp->D7, (val >> 3) & 1) ||
			gpio_value(disp->D6, (val >> 2) & 1) ||
			gpio_value(disp->D5, (val >> 1) & 1) ||
			gpio_value(disp->D4, val & 1)) {
		return 1;
	}

	// Clock the LCD and return the error status
	return LCD_clock(disp, t);
}

/* Initialize the LCD connected to the pins listed in disp */
int LCD_init(const struct LCD *disp)
{
	// The required GPIO pins need to be configured
	if (LCD_gpio_init(disp))
		return 1;
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
	if (LCD_instruction_write(disp, LCD_ENTRY_MODE | LCD_ENTRY_MODE_ID, 40))
		return 1;

	// Turn on the display, cursor, and blinking
	return LCD_instruction_write(disp, LCD_ON_OFF_CTRL | LCD_ON_OFF_CTRL_D |
			LCD_ON_OFF_CTRL_C | LCD_ON_OFF_CTRL_B, 40);
}

/* Unexport all the gpio pins associated with disp */
int LCD_deinit(const struct LCD *disp)
{
	int r;

	// Export the required pins
	r = gpio_unexport(disp->RS);
	r |= gpio_unexport(disp->E);
	r |= gpio_unexport(disp->D4);
	r |= gpio_unexport(disp->D5);
	r |= gpio_unexport(disp->D6);
	r |= gpio_unexport(disp->D7);
	return r;
}


/* Write data to the LCD at the current DD ram address */
int LCD_data_write(const struct LCD *disp, char letter)
{
	if (letter < 0x20)
		return 2;

	// Write the first half at the cursor location
	if (LCD_write(disp, 0x10 | ((letter & 0xF0) >> 4), 0))
		return -1;

	// Write the second half at the cursor location
	return LCD_write(disp, 0x10 | (letter & 0xF), 40);

}

/* Write an instruction to the LCD */
int LCD_instruction_write(const struct LCD *disp, char instr, unsigned int t)
{
	char data = (instr & 0xF0) >> 4; // Get the first part of instr

	// Check for the FNCT_SET init instructions, which act differently
	if ((instr == (LCD_FNCT_SET | LCD_FNCT_SET_DL | LCD_FNCT_INIT)) ||
			(instr == (LCD_FNCT_SET | LCD_FNCT_INIT))) {
		// Write the instruction to the LCD
		return LCD_write(disp, data, t);
	}

	// Normal instruction writing
	// Write the first half
	if (LCD_write(disp, data, 1))
		return 1;

	// Write the second half
	return LCD_write(disp, instr & 0x0F, t);
}

/*
 * Writes character a to the location specified by row and col to the
 * display specified by disp. Row must be 0 or 1 and col must be from 0-15.
 */
int LCD_character_write(const struct LCD *disp, char a, char row, char col)
{
	char addr;

	// Make sure the character and location are valid
	if (a < 0x20 || (row != 0 && row != 1) || (col < 0 || col > 15))
		return 1;

	// Combine the locations into the address
	addr = (row << 6) | col;

	// Set the DD ram address to the location pointed to by row and col
	if (LCD_instruction_write(disp, LCD_SET_DD_ADDR | addr, 40))
		return 1;

	return LCD_data_write(disp, a); // Finally, write requested character
}

