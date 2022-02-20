#ifndef GPIO_Q4_H
#define GPIO_Q4_H

/* Exports the passed GPIO pin */
int gpio_export(int pin);

/* Unexports the passed GPIO pin */
int gpio_unexport(int pin);

/* Sets the passed pin to either high or low */
int gpio_value(int pin, int value);

/* Sets the passed pin as input or output */
int gpio_direction(int pin, char *direction);

#endif /* GPIO_Q4_H */
