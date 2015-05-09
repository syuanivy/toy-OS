
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
static void task_1(void *);
static void task_2(void *);
static void task_3(void *);
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


tid_t tmp_tid = 0;
static void init_all_threads() {
    lock_init(&lock_task);
    tmp_tid = thread_create("Super Duper", PRI_MAX, &task_1, NULL);
    thread_create("Burger King", PRI_MAX, &task_2, &tmp_tid);
    thread_create("In and Out", PRI_MAX, &task_3, &tmp_tid);
}

static void task_1(void *param UNUSED) {
    int i = 0;
    while(i < 100) {
        printf("yeah~I'm the target!\n");
        ++i;
    }
}

static void task_2(void *param UNUSED) {
    tid_t *target_tid = param;
    thread_wait(*target_tid);
}

static void task_3(void *param UNUSED) {
    tid_t *target_tid = param;
    thread_wait(*target_tid);
}

