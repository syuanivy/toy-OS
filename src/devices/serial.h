#ifndef DEVICES_SERIAL_H
#define DEVICES_SERIAL_H

#include <stdint.h>

#define CR 13
#define BS 8

void serial_init(void);
void serial_putc (char);
void serial_flush (void);
void serial_notify (void);

void uart_putc(unsigned char byte);
unsigned char uart_getc();
void uart_write(const char* buffer, size_t size);
void uart_puts(const char* str);
void test_serial();
void hexstrings(unsigned int d);

#endif /* devices/serial.h */
