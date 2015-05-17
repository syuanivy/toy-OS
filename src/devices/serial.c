
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "serial.h"
#include "bcm2835.h"
#include "interrupt.h"
#include "thread.h"

#define RXBUFMASK 0xFFF// a buffer of 4095 bytes at max
volatile static unsigned int input_head;
volatile static unsigned int input_tail;
volatile static unsigned char input_buffer[RXBUFMASK + 1];

static struct thread *uart_blocking_thread = NULL;

void hexstrings(unsigned int d) {
    //unsigned int ra;
    unsigned int rb;
    unsigned int rc;

    rb = 32;
    while (1) {
        rb -= 4;
        rc = (d >> rb)&0xF;
        if (rc > 9) rc += 0x37;
        else rc += 0x30;
        uart_putc(rc);
        if (rb == 0) break;
    }
    uart_putc(0x20);
}

void serial_init(void) {
    test_serial();
}

void serial_putc(char character) {
    //uart_putc(96);
    //uart_putc('B');
    uart_putc(character);
}

void serial_flush(void) {
    // TODO Implement the method.
}

void serial_notify(void) {
    // TODO Implement the method.
}

static inline void mmio_write(uint32_t reg, uint32_t data) {
    uint32_t *ptr = (uint32_t*) reg;
    asm volatile("str %[data], [%[reg]]" : : [reg]"r"(ptr), [data]"r"(data));
}

static inline uint32_t mmio_read(uint32_t reg) {
    uint32_t *ptr = (uint32_t*) reg;
    uint32_t data;
    asm volatile("ldr %[data], [%[reg]]" : [data]"=r"(data) : [reg]"r"(ptr));
    return data;
}

/* Loop <delay> times in a way that the compiler won't optimize away. */
static inline void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
            : : [count]"r"(count) : "cc");
}

enum {
    // The GPIO registers base address.
    GPIO_BASE = 0x20200000,

    // The offsets for reach register.

    // Controls actuation of pull up/down to ALL GPIO pins.
    GPPUD = (GPIO_BASE + 0x94),

    // Controls actuation of pull up/down for specific GPIO pin.
    GPPUDCLK0 = (GPIO_BASE + 0x98),

    // The base address for UART.
    UART0_BASE = 0x20201000,

    // The offsets for reach register for the UART.
    UART0_DR = (UART0_BASE + 0x00),
    UART0_RSRECR = (UART0_BASE + 0x04),
    UART0_FR = (UART0_BASE + 0x18),
    UART0_ILPR = (UART0_BASE + 0x20),
    UART0_IBRD = (UART0_BASE + 0x24),
    UART0_FBRD = (UART0_BASE + 0x28),
    UART0_LCRH = (UART0_BASE + 0x2C),
    UART0_CR = (UART0_BASE + 0x30),
    UART0_IFLS = (UART0_BASE + 0x34),
    UART0_IMSC = (UART0_BASE + 0x38),
    UART0_RIS = (UART0_BASE + 0x3C),
    UART0_MIS = (UART0_BASE + 0x40),
    UART0_ICR = (UART0_BASE + 0x44),
    UART0_DMACR = (UART0_BASE + 0x48),
    UART0_ITCR = (UART0_BASE + 0x80),
    UART0_ITIP = (UART0_BASE + 0x84),
    UART0_ITOP = (UART0_BASE + 0x88),
    UART0_TDR = (UART0_BASE + 0x8C),
};

/* uart interrupt handler */
static void uart_irq_handler(struct interrupts_stack_frame *stack_frame) {
    // we only deal with masked interrupts here.
    uint32_t masked_irq_status = mmio_read(UART0_MIS);
    // using receive timeout interrupt for unbuffered input.
    if ((masked_irq_status & (1 << 6)) == 0x40) {
        int c = mmio_read(UART0_DR);
        // looks like the key mapping for BCM2835 is not work as expected,
        // manually map keystrokes for backspace here.
        if (c == 0x7F) {
            c = 0x08;//BS
        }
        input_buffer[input_head] = c & 0xFF;
        input_head = (input_head + 1) & RXBUFMASK;
        if (uart_blocking_thread != NULL && 
                uart_blocking_thread->status == THREAD_BLOCKED) {
            thread_unblock(uart_blocking_thread);
        }
    }
    return;
}

void uart_init() {
    // Disable UART0.
    mmio_write(UART0_CR, 0x00000000);
    // Setup the GPIO pin 14 && 15.

    // Disable pull up/down for all GPIO pins & delay for 150 cycles.
    mmio_write(GPPUD, 0x00000000);
    delay(150);

    // Disable pull up/down for pin 14,15 & delay for 150 cycles.
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay(150);

    // Write 0 to GPPUDCLK0 to make it take effect.
    mmio_write(GPPUDCLK0, 0x00000000);

    // Clear pending interrupts.
    mmio_write(UART0_ICR, 0x7FF);

    // Set integer & fractional part of baud rate.
    // Divider = UART_CLOCK/(16 * Baud)
    // Fraction part register = (Fractional part * 64) + 0.5
    // UART_CLOCK = 3000000; Baud = 115200.

    // Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
    // Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
    mmio_write(UART0_IBRD, 1);
    mmio_write(UART0_FBRD, 40);

    // Enable FIFO & 8 bit data transmission (1 stop bit, no parity).
    mmio_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));

    // Mask RXIM, TXIM, RTIM, this determines which interrupt
    // is enabled. (Even if we don't enable it, we still can get it in
    // the RIS register.)
    mmio_write(UART0_IMSC, (1 << 4) | (1 << 6));

    // FIFO receive interrupt trigger level (1/8))
    //    mmio_write(UART0_IFLS, UART0_IFLS & 0xffffff37);

    // Enable UART0, receive & transmit.
    mmio_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));

    // register UART interrupt handler
    interrupts_register_irq(IRQ_UART, uart_irq_handler, "UART Interrupt");
}

/* Interrupt-Driven Output Routine 
 * This function simply puts output character
 * into the output buffer. Interrupt handler will
 * manage to output those buffered characters 
 * to the serial port. This function acts as a producer
 * of the output buffer. The current version is not
 * very robust, since we are not dealing with overflow
 * here.
 */
void uart_putc_helper(unsigned char byte) {
    while ( mmio_read(UART0_FR) & (1 << 5) ) { }
    mmio_write(UART0_DR, byte);
}

void uart_putc(unsigned char byte) {
    if (byte == '\n') {
        uart_putc_helper('\n');
        uart_putc_helper('\r');
    } else {
        uart_putc_helper(byte);
    }
}

/* Interrupt-Driven Input Routine 
 * This function blocks when no data available in the 
 * input buffer, until the interrupt handler puts data
 * into the buffer and unblock the blocked thread. This
 * function acts as a consumer of the input buffer. The
 * current version is not robust because we are not 
 * dealing with overflow here.
 */
unsigned char uart_getc() {
    // if the buffer is not empty, return data,
    // else block. the current thread will only be woke up
    // by uart interrupt handler.
    // for now we should only use this function in the thread running shell.
    if (input_tail == input_head) {
        // IMPORTANT: setup our blocking thread variable before blocking.
        uart_blocking_thread = thread_current();
        // disable interrupt and block the current thread.
        interrupts_disable();
        thread_block();
    }
    // consume one character (byte)
    unsigned char c = input_buffer[input_tail];
    input_tail = (input_tail + 1) & RXBUFMASK;
    return c;
}

void uart_write(const char* buffer, size_t size) {
    size_t i;
    for (i = 0; i < size; i++)
        uart_putc(buffer[i]);
}

void uart_puts(const char* str) {
    uart_write((const unsigned char*) str, strlen(str));
}

void test_serial() {

    uart_init();
}
