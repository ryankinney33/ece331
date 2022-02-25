#ifndef LCD_H
#define LCD_H

/* Macros for the required GPIO pins */
#define LCD_RS	17
#define LCD_E	27
#define DATA_D4	6
#define DATA_D5 13
#define DATA_D6 19
#define DATA_D7 26

/*
 * Macros for the display instructions. Please see
 * the Optrex Character LCD Manual for more information.
 */
#define LCD_CLR_DISP	0x01 // Clears entire display
#define LCD_RET_HOME	0x02 // Returns shifted display to origin

#define LCD_ENTRY_MODE	  0x04 // Defaults to decrement and no shift
#define LCD_ENTRY_MODE_ID 0x02 // Sets direction to increment
#define LCD_ENTRY_MODE_S  0x01 // Enables display shift

#define LCD_ON_OFF_CTRL   0x08 // Base on/off instruction
#define LCD_ON_OFF_CTRL_D 0x04 // Entire Display ON/OFF
#define LCD_ON_OFF_CTRL_C 0x02 // Cursor ON/OFF
#define LCD_ON_OFF_CTRL_B 0x01 // Blink ON/OFF

#define LCD_CRSR_DISP_SHFT      0x10 // Defaults to cursor move left
#define LCD_CRSR_DISP_SHFT_SC	0x08 // Set display shift
#define LCD_CRSR_DISP_SHFT_RL	0x04 // Set direction to right

#define LCD_FNCT_SET	0x20	// Default to 4 bit, 1 line, 5x7 dots
#define LCD_FNCT_SET_DL	0x10	// Set to 8 bit mode
#define LCD_FNCT_SET_N	0x08	// Set to 2 line mode
#define LCD_FNCT_SET_F	0x04	// Set to 5x10 dot mode

#define LCD_SET_CG_ADDR	0x40	// Default address is 0x00
#define LCD_SET_DD_ADDR 0x80	// Default address is 0x00

/* Initialize the LCD */
int LCD_init();

/* Write data to the LCD */
int LCD_data_write();

/* Write an instruction to the LCD */
int LCD_instruction_write(char instruction);

/* Write a character to a specific location on the display */
int LCD_character_write();

#endif /* LCD_H */
