/*
 * gpio.h
 *
 * Contains routines for the manipulation of the GPIO ports.
 *
 *  Created on: Nov 28, 2014
 *      Author: jcyescas
 */
#ifndef DEVICES_GPIO_H_
#define DEVICES_GPIO_H_

/* Sets the function of the GPIO register. */
void gpio_enable_function(unsigned int gpio_register, unsigned int function);

/* Sets the GPIO pin addressed to high if value is different than zero and
 * low otherwise.*/
void gpio_set_register(unsigned int gpio_register, unsigned int value);

#endif /* DEVICES_GPIO_H_ */
