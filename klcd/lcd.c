// 3/03/2022 - LCD Driver
// Ryan Kinney <ryan.kinney@maine.edu>
// Based on framework code from A. Sheaff <sheaff@maine.edu>
// Kernel driver that operates an LCD
// Data is written with write
// Instructions can be written with ioctl
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/consumer.h>
#include <linux/gpio/consumer.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>

#include "lcd.h"

// Data to be "passed" around to various functions
struct lcd_data_t {
	struct gpio_desc *gpio_lcd_rs;		// LCD RS pin
	struct gpio_desc *gpio_lcd_e;		// LCD E pin
	struct gpio_desc *gpio_lcd_d4;		// LCD D4 pin
	struct gpio_desc *gpio_lcd_d5;		// LCD D5 pin
	struct gpio_desc *gpio_lcd_d6;		// LCD D6 pin
	struct gpio_desc *gpio_lcd_d7;		// LCD D7 pin
	struct class *lcd_class;			// Class for auto /dev population
	struct device *lcd_dev;				// Device for auto /dev population
	struct platform_device *pdev;		// Platform driver
	int lcd_major;						// Device major number
	s8 lcd_row;						// LCD cursor row
	s8 lcd_col;						// LCD cursor column
	s8 lcd_dir;						// LCD cursor move direction
	int numcols;						// Number of columns on the LCD
	struct mutex lock;				// Mutual exclusion lock
};

// LCD data structure access between functions
static struct lcd_data_t *lcd_dat=NULL;

// Clocks the LCD display to write the data
static void lcd_clock(unsigned int t)
{
	// Pull the E pin high to send the instruction
	gpiod_set_value(lcd_dat->gpio_lcd_e, 1);
	fsleep(5);

	gpiod_set_value(lcd_dat->gpio_lcd_e, 0); // Pull E low for the next instruction
	fsleep(t); // Wait the required time for the instruction execution
}

/*
 * Sets the RS and data GPIO then sets the E pin to write to the display.
 * The bits of val represent RS,D7,D6,D5,D4
 * t is the delay to wait after writing the instruction
 */
static void lcd_pin_set(char val, unsigned long t)
{
	// Decode val and set pins accordingly
	gpiod_set_value(lcd_dat->gpio_lcd_rs, (val >> 4) & 1);
	gpiod_set_value(lcd_dat->gpio_lcd_d4, val  & 1);
	gpiod_set_value(lcd_dat->gpio_lcd_d5, (val >> 1) & 1);
	gpiod_set_value(lcd_dat->gpio_lcd_d6, (val >> 2) & 1);
	gpiod_set_value(lcd_dat->gpio_lcd_d7, (val >> 3) & 1);

	// Clock the LCD
	lcd_clock(t);
}

// Forward declaration for lcd_position_fix to compile
static void lcd_instr_write(char instr, unsigned long t);

// Handles line wrapping with the cursor position
static void lcd_position_fix(void)
{
	// Perform bounds checking on the column
	if (lcd_dat->lcd_col >= lcd_dat->numcols) {
		lcd_dat->lcd_col = 0; // Went off right edge
		lcd_dat->lcd_row = 1 - lcd_dat->lcd_row; // toggle row
		goto update;
	} else if (lcd_dat->lcd_col < 0) {
		lcd_dat->lcd_col = lcd_dat->numcols - 1; // Went off left edge
		lcd_dat->lcd_row = 1 - lcd_dat->lcd_row; // toggle row
		goto update;
	}
	return;
update:
	// Change the cursor postion on the LCD
	lcd_instr_write(LCD_SET_DD_ADDR | (lcd_dat->lcd_row << 6) | lcd_dat->lcd_col, 0);
}

/*
 * Write an instruction to the LCD.
 * Argument t is used for the delay. If t is 0, the delay
 * is determined based on the instruction being written.
 */
static void lcd_instr_write(char instr, unsigned long t)
{
	char data = (instr & 0xF0) >> 4; // Gets first half

	// Determine the length to delay based on the instruction (if needed)
	if (!t) {
		if (instr == LCD_CLR_DISP || (instr & 0xFE) == LCD_RET_HOME) {
			t = 15200; // 15.2 ms delay for return home or clear
			lcd_dat->lcd_row = 0; // reset the cursor position
			lcd_dat->lcd_col = 0;
			lcd_dat->lcd_dir = 1; // also resets the direction
		} else {
			t = 40; // 40 us delay for everything else
		}
	}

	// Check for the FNCT_SET init instructions, which act differently
	if ((instr & 0xE0) == LCD_FNCT_SET && (instr & 0x3) == LCD_FNCT_INIT) {
		// Write the instruction to the LCD
		lcd_pin_set(data, t);
		return;
	}

	// Normal instruction writing
	lcd_pin_set(data, 10); // First half
	lcd_pin_set(instr & 0x0F, t); // Second half

	// Update the cursor data based on the instruction
	if ((instr & 0xF0) == LCD_CRSR_DISP_SHFT) { // cursor move instruction
		if (!(instr & LCD_CRSR_DISP_SHFT_SC)) {
			if (instr & LCD_CRSR_DISP_SHFT_RL) {
				lcd_dat->lcd_col++; // move right
			} else {
				lcd_dat->lcd_col--; // move left
			}

			// Tell the LCD the new position
			lcd_position_fix();
		}
	} else if ((instr & 0xFC) == LCD_ENTRY_MODE) { // change direction?
		if (instr & LCD_ENTRY_MODE_ID) {
			lcd_dat->lcd_dir = 1; // cursor is incremented
		} else {
			lcd_dat->lcd_dir = -1; // cursor is decremented
		}
	} else if (instr & LCD_SET_DD_ADDR) { // change cursor location
		lcd_dat->lcd_col = instr & 0x3F;
		lcd_dat->lcd_row = (instr >> 6) & 1;
		lcd_position_fix();
	}
}

// Write data to the LCD at the current DD ram address
static void lcd_data_write(unsigned char a)
{
	// Write the first half at the cursor location
	lcd_pin_set(0x10 | ((a & 0xF0) >> 4), 10);

	// Write the second half at the cursor location
	lcd_pin_set(0x10 | (a & 0xF), 40);

	// Update the cursor location
	lcd_dat->lcd_col += lcd_dat->lcd_dir;
	lcd_position_fix(); // handle wrapping
}

// Initialize the LCD connected to the pins listed in lcd_dat
static void lcd_init(void)
{
	// Send Function Set 8 command
	lcd_instr_write(LCD_FNCT_SET | LCD_FNCT_SET_DL | LCD_FNCT_INIT, 4100);

	// Send another Function Set 8 command
	lcd_instr_write(LCD_FNCT_SET | LCD_FNCT_SET_DL | LCD_FNCT_INIT, 100);

	// Send the final Function Set 8 command
	lcd_instr_write(LCD_FNCT_SET | LCD_FNCT_SET_DL | LCD_FNCT_INIT, 0);

	// Send a Function Set 4 command
	lcd_instr_write(LCD_FNCT_SET | LCD_FNCT_INIT, 0);

	// Configure the display; 2 lines, 5x7 font
	lcd_instr_write(LCD_FNCT_SET | LCD_FNCT_SET_N, 0);

	// Turn the display off
	lcd_instr_write(LCD_ON_OFF_CTRL, 0);

	// Clear the display
	lcd_instr_write(LCD_CLR_DISP, 0);

	// Set the entry mode to increment without display shift
	lcd_instr_write(LCD_ENTRY_MODE | LCD_ENTRY_MODE_ID, 0);

	// Turn on the display, cursor, and blinking
	lcd_instr_write(LCD_ON_OFF_CTRL | LCD_ON_OFF_CTRL_D |
			LCD_ON_OFF_CTRL_C | LCD_ON_OFF_CTRL_B, 0);
}

// ioctl system call
// If an LCD instruction is the command then
//   use arg directly as the value (8 bits) and write to the LCD as an instruction
// Determine if locking is needed and add appropriate code as needed.
static long lcd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LCD_IOC_INSTR:
		// Wait until the lock is acquired to not interfere with other
		// processes
		if (mutex_lock_interruptible(&lcd_dat->lock))
			return -EINTR; // interrupted; return an error

		// Write the requested instruction to the LCD before unlocking
		lcd_instr_write(arg, 0);
		mutex_unlock(&lcd_dat->lock);
		return 0;
	default:
		return -ENOTTY; // not a valid LCD ioctl command
	}
}

// Write system call - always check for and act appropriately upon error.
//  DO not return upon while holding a lock
// maximum write size to the LCD is 32 bytes.
//   Do not allow any more.  Return the minimum of count or 32, or an error.
// allocate memory in the kernel for data passed from user space
// copy the data
// If the file is opened non-blocking and the lock is held,
//   return with an appropraite error
// else
//   If the lock is held,
//     Wait until the lock is released and then acquire the lock
//     if while waiting for the lock, the user process is interrupted
//       return with and error
//   else
//     Acquire the lock
// Write up to the first 32 bytes of data to the LCD at the proper location
// release the lock
// Free memory
// This function return the number of byte successfully writen
static ssize_t lcd_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *offp)
{
	char *buffer;
	size_t i;

	if (buf == NULL) {
		return -EFAULT;
	}

	// Handle locking
	if (filp->f_flags & O_NONBLOCK) {
		// File opened non-blocking; return error if lock is held
		if (!mutex_trylock(&lcd_dat->lock)) {
			// lock not acquired
			return -EAGAIN;
		}
	} else {
		// Blocking I/O; wait until lock is acquired unless interrupted
		if (mutex_lock_interruptible(&lcd_dat->lock)) {
			// Interrupted while waiting for lock
			return -EINTR;
		}
	} // Lock is acquired, proceed

	// Count must not go over the maximum
	if (count > (lcd_dat->numcols << 1)) {
		count = lcd_dat->numcols << 1;
	}

	// Attempt to allocate memory and copy buf
	buffer = kmalloc(count * sizeof(char), GFP_KERNEL);
	if (buffer == NULL) {
		printk(KERN_INFO "Memory allocation failed\n");
		mutex_unlock(&lcd_dat->lock);
		return -ENOMEM;
	}
	// num copied = num to copy - num not copied
	count -= copy_from_user(buffer, buf, count);

	// Write the data to the LCD
	for (i = 0; i < count; ++i) {
		lcd_data_write(buffer[i]);
	}

	// Cleanup and return
	mutex_unlock(&lcd_dat->lock);
	kfree(buffer);
	return count;
}

// Open system call
// Open only if the file access flags (NOT permissions) are appropiate as discussed in class
// Return an appropraite error otherwise
static int lcd_open(struct inode *inode, struct file *filp)
{
	if (filp->f_flags & O_WRONLY) // Must be opened write only
		return 0;
	return -EACCES; // Invalid access mode; must be write only
}

// Close system call
static int lcd_release(struct inode *inode, struct file *filp)
{
	return 0; // No work to be done...
}

// File operations for the lcd device.  Uninitialized will be NULL.
static const struct file_operations lcd_fops = {
	.owner = THIS_MODULE,	// Us
	.open = lcd_open,		// Open
	.release = lcd_release,// Close
	.write = lcd_write,	// Write
	.unlocked_ioctl=lcd_ioctl,	// ioctl
};

// Init value<0 means input
static struct gpio_desc *lcd_obtain_pin(struct device *dev, int pin, char *name, int init_val)
{
	struct gpio_desc *gpiod_pin=NULL;	// GPIO Descriptor for setting value
	int ret=-1;	// Return value

	// Request the pin - release with devm_gpio_free() by pin number
	if (init_val>=0) {
		ret=devm_gpio_request_one(dev,pin,GPIOF_OUT_INIT_LOW,name);
		if (ret<0) {
			dev_err(dev,"Cannot get %s gpio pin\n",name);
			gpiod_pin=NULL;
			goto fail;
		}
	} else {
		ret=devm_gpio_request_one(dev,pin,GPIOF_IN,name);
		if (ret<0) {
			dev_err(dev,"Cannot get %s gpio pin\n",name);
			gpiod_pin=NULL;
			goto fail;
		}
	}

	// Get the gpiod pin struct
	gpiod_pin=gpio_to_desc(pin);
	if (gpiod_pin==NULL) {
		printk(KERN_INFO "Failed to acquire lcd gpio\n");
		gpiod_pin=NULL;
		goto fail;
	}

	// Make sure the pin is set correctly
	if (init_val>=0) gpiod_set_value(gpiod_pin,init_val);

	return gpiod_pin;

fail:
	if (ret>=0) devm_gpio_free(dev,pin);

	return gpiod_pin;
}


// Sets device node permission on the /dev device special file
static char *lcd_devnode(struct device *dev, umode_t *mode)
{
	if (mode) *mode = S_IRUGO | S_IWUGO; // read+write, user group other
	return NULL;
}

// This is called on module load.
static int __init lcd_probe(void)
{
	int ret=-1;	// Return value

	// Allocate device driver data and save
	lcd_dat=kmalloc(sizeof(struct lcd_data_t),GFP_KERNEL);
	if (lcd_dat==NULL) {
		printk(KERN_INFO "Memory allocation failed\n");
		return -ENOMEM;
	}

	memset(lcd_dat,0,sizeof(struct lcd_data_t));

	// Create the device - automagically assign a major number
	lcd_dat->lcd_major=register_chrdev(0,"lcd",&lcd_fops);
	if (lcd_dat->lcd_major<0) {
		printk(KERN_INFO "Failed to register character device\n");
		ret=lcd_dat->lcd_major;
		goto fail;
	}

	// Create a class instance
	lcd_dat->lcd_class=class_create(THIS_MODULE, "lcd_class");
	if (IS_ERR(lcd_dat->lcd_class)) {
		printk(KERN_INFO "Failed to create class\n");
		ret=PTR_ERR(lcd_dat->lcd_class);
		goto fail;
	}

	// Setup the device so the device special file is created with 0666 perms
	lcd_dat->lcd_class->devnode=lcd_devnode;
	lcd_dat->lcd_dev=device_create(lcd_dat->lcd_class, NULL,
			MKDEV(lcd_dat->lcd_major, 0), (void *)lcd_dat, "lcd");
	if (IS_ERR(lcd_dat->lcd_dev)) {
		printk(KERN_INFO "Failed to create device file\n");
		ret=PTR_ERR(lcd_dat->lcd_dev);
		goto fail;
	}

	lcd_dat->gpio_lcd_rs=lcd_obtain_pin(lcd_dat->lcd_dev,17,"LCD_RS",0);
	if (lcd_dat->gpio_lcd_rs==NULL) goto fail;
	lcd_dat->gpio_lcd_e=lcd_obtain_pin(lcd_dat->lcd_dev,27,"LCD_E",0);
	if (lcd_dat->gpio_lcd_e==NULL) goto fail;
	lcd_dat->gpio_lcd_d4=lcd_obtain_pin(lcd_dat->lcd_dev,6,"LCD_D4",0);
	if (lcd_dat->gpio_lcd_d4==NULL) goto fail;
	lcd_dat->gpio_lcd_d5=lcd_obtain_pin(lcd_dat->lcd_dev,13,"LCD_D5",0);
	if (lcd_dat->gpio_lcd_d5==NULL) goto fail;
	lcd_dat->gpio_lcd_d6=lcd_obtain_pin(lcd_dat->lcd_dev,19,"LCD_D5",0);
	if (lcd_dat->gpio_lcd_d6==NULL) goto fail;
	lcd_dat->gpio_lcd_d7=lcd_obtain_pin(lcd_dat->lcd_dev,26,"LCD_D7",0);
	if (lcd_dat->gpio_lcd_d7==NULL) goto fail;

	mutex_init(&lcd_dat->lock);
	mutex_lock(&lcd_dat->lock); // Acquire the lock then initialize the LCD

	lcd_dat->lcd_row = 0;
	lcd_dat->lcd_col = 0;
	lcd_dat->numcols = 16; // assume the user is using a 20 column lcd
	lcd_init();

	mutex_unlock(&lcd_dat->lock); // Release the lock
	printk(KERN_INFO "Registered\n");
	return 0;

fail:
	if (lcd_dat->gpio_lcd_d7)
		devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_d7));
	if (lcd_dat->gpio_lcd_d6)
		devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_d6));
	if (lcd_dat->gpio_lcd_d5)
		devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_d5));
	if (lcd_dat->gpio_lcd_d4)
		devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_d4));
	if (lcd_dat->gpio_lcd_e)
		devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_e));
	if (lcd_dat->gpio_lcd_rs)
		devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_rs));

	// Device cleanup
	if (lcd_dat->lcd_dev) device_destroy(lcd_dat->lcd_class,MKDEV(lcd_dat->lcd_major,0));
	// Class cleanup
	if (lcd_dat->lcd_class) class_destroy(lcd_dat->lcd_class);
	// char dev clean up
	if (lcd_dat->lcd_major) unregister_chrdev(lcd_dat->lcd_major,"lcd");

	if (lcd_dat!=NULL) kfree(lcd_dat);
	printk(KERN_INFO "LCD Failed\n");
	return ret;
}

// Called when the module is removed
static void __exit lcd_remove(void)
{
	// Free the gpio pins with devm_gpio_free() & gpiod_put()
	devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_d7));
	devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_d6));
	devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_d5));
	devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_d4));
	devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_e));
	devm_gpio_free(lcd_dat->lcd_dev,desc_to_gpio(lcd_dat->gpio_lcd_rs));

	// Device cleanup
	device_destroy(lcd_dat->lcd_class,MKDEV(lcd_dat->lcd_major,0));
	// Class cleanup
	class_destroy(lcd_dat->lcd_class);
	// Remove char dev
	unregister_chrdev(lcd_dat->lcd_major,"lcd");

	// Free the device driver data
	if (lcd_dat!=NULL) {
		kfree(lcd_dat);
		lcd_dat=NULL;
	}

	printk(KERN_INFO "Removed\n");
}

module_init(lcd_probe);
module_exit(lcd_remove);

MODULE_DESCRIPTION("RPI LCD");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LCD");

