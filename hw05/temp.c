#include <unistd.h>

#include "gpio.h"

/* Macros for the required GPIO pins */
#define LCD_RS	17
#define LCD_E	27
#define DATA_D4	6
#define DATA_D5 13
#define DATA_D6 19
#define DATA_D7 26

// Only used during initialization
#define LCD_FUNCTION_SET4 0x20
#define LCD_FUNCTION_SET8 0x30


#define LCD_FUNCTION_SET  0x2C
#define LCD_DISPLAY_OFF	  0x08
#define LCD_CLEAR_DISPLAY 0x01
#define LCD_ENTRY_MODE    0x0

/* Initialize the LCD */
int LCD_init() {
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

	usleep(15010); // Delay for more than 15 ms after Vcc = 4.5V
	if (LCD_instruction_write(LCD_FUNCTION_SET8))
		return 1;

	usleep(4110); // Delay for more than 4.1 ms
	if (LCD_instruction_write(LCD_FUNCTION_SET8))
		return 1;

	usleep(110); // Delay for more than 100 microseconds
	if (LCD_instruction_write(LCD_FUNCTION_SET8))
		return 1;
	if(LCD_instruction_write(LCD_FUNCTION_SET4))
		return 1;

}

/* Write data to the LCD */
int LCD_data_write();

/* Write an instruction to the LCD */
int LCD_instruction_write(char instruction);

/* Write a character to a specific location on the display */
int LCD_character_write();
