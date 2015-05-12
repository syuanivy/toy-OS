#ifndef DEVICES_SERIAL_H
#define DEVICES_SERIAL_H

#include <stdint.h>

void serial_init(void);
void serial_putc (char);
void serial_flush (void);
void serial_notify (void);

void uart_putc(unsigned char byte);
unsigned char uart_getc();
void test_serial();

#endif /* devices/serial.h */
