#ifndef _GPIO_Q4_H_
#define _GPIO_Q4_H_

int gpio_export(int pin);
int gpio_unexport(int pin);
int gpio_value(int pin, int value);
int gpio_direction(int pin, char *direction);

#endif /* _GPIO_Q4_H_ */
