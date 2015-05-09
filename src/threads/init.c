
#include <debug.h>
#include <random.h>
#include <stdbool.h>
#include "stdio.h"
#include <stdint.h>
#include <string.h>

#include "../devices/gpio.h"
#include "../devices/framebuffer.h"
#include "../devices/serial.h"
#include "../devices/timer.h"
#include "../devices/video.h"
#include "interrupt.h"
#include "init.h"
#include "palloc.h"
#include "malloc.h"
#include "synch.h"
#include "thread.h"
#include "vaddr.h"

/* -ul: Maximum number of pages to put into palloc's user pool. */
static size_t user_page_limit = SIZE_MAX;

/* Tasks for the Threads. */
static void task_0(void *);
static void task_1(void *);
static void task_2(void *);
static void task_3(void *);
static void task_4(void *);
static void task_5(void *);
static void task_6(void *);
static void init_all_threads();
static struct lock lock_task;

/*
 * kernel.c
 *
 *  Created on: Oct 22, 2014
 *      Author: jcyescas
 */

static void test_swi_interrupt() {
    unsigned short green = 0x7E0;
    SetForeColour(green);
    generate_swi_interrupt(); // Function defined in interrupts.s
}

/* Initializes the Operating System. The interruptions have to be disabled at entrance.
 *
 *  - Sets interruptions
 *  - Sets the periodic timer
 *  - Set the thread functionality
 */
void init() {

    /* Initializes ourselves as a thread so we can use locks,
      then enable console locking. */
    thread_init();

    /* Initializes the frame buffer and console. */
    framebuffer_init();
    serial_init();
    video_init();

    printf("\nosOs Kernel Initializing");

    /* Initialize memory system. */
    palloc_init(user_page_limit);
    malloc_init();

    /* Initializes the Interrupt System. */
    interrupts_init();
    timer_init();

    timer_msleep(5000000);

    /* Starts preemptive thread scheduling by enabling interrupts. */
    thread_start();

    printf("\nFinish booting.");

    init_all_threads();

    int i = 0;
    while (true) {
        enum interrupts_level old_level = interrupts_disable();
        unsigned short red = 0xF800;
        unsigned short green = 0x7E0;
        SetForeColour(red + green);
        printf("\nosOs v0.0 Forever: ");
        printf(" Thread: %s", thread_current()->name);
        printf(", Priority: %d", thread_current()->priority);

        interrupts_set_level(old_level);
    }

    thread_exit();
}

static void init_all_threads() {
    lock_init(&lock_task);
    tid_t target_tid = thread_create("Thread 1", PRI_MAX, &task_1, NULL);
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ tid: %u\n", target_tid);
    printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ pointer tid: %u\n", target_tid); 
    thread_create("Thread 2", PRI_MAX, &task_2, &target_tid);
}

static void task_1(void *param) {
    while (1) {
        printf("yeah~I'm the target!\n");
    }
}

static void task_2(void *param) {
    tid_t *target_tid = param;
    printf("......................tid: %u\n", *target_tid);
    thread_wait(*target_tid);
}


/* Task 6 calculates the factorial. */
static void task_6(void *param) {
    unsigned short blue = 0x1f;
    unsigned short green = 0x7E0;

    int i = 1;
    while (i++ < 250) {
        int number = i % 25;
        int fac1 = factorial(number);
        int fac2 = factorial(number);

        ASSERT(fac1 == fac2);
        SetForeColour(green + blue);
        printf("\n%s - Factorial(%d) = %d", thread_current()->name, number, fac1);
    }
}

