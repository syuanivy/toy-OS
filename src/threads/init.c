
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
#include "../devices/bcm2835.h"
#include "interrupt.h"
#include "init.h"
#include "palloc.h"
#include "malloc.h"
#include "synch.h"
#include "thread.h"
#include "vaddr.h"
#include "shell.h"

/* -ul: Maximum number of pages to put into palloc's user pool. */
static size_t user_page_limit = SIZE_MAX;

/* Tasks for the Threads. */
static void task_1(void *);
static void task_2(void *);
static void task_3(void *);
static void task_shell(void *);
static void init_all_threads();
static struct lock lock_task;

/*Tasks for Testing Non-busy Waiting*/
static void init_busy_test();
static void init_nonbusy_test();
static void task_busy_sleeper(void *);
static void task_nonbusy_sleeper(void *);
static void task_runner(void *);
static void task_always(void *);
static struct lock lock_task_busy;
static struct lock lock_task_nonbusy;

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

    /* Initializes the Interrupt System, we do that before initializing
     * serial port */
    interrupts_init();

    /* Initializes the frame buffer and console. */
    framebuffer_init();
    serial_init();
    video_init();

    /* Initialize memory system. */
    palloc_init(user_page_limit);
    malloc_init();


    timer_init();
    printf("\n[Kernel Initialization Finished]\n");

    /* Starts preemptive thread scheduling by enabling interrupts. */
    thread_start();

    init_all_threads();

    while (true) {
    }

    thread_exit();
}


tid_t tmp_tid = 0;

static void init_all_threads() {
    lock_init(&lock_task);
    //    tmp_tid = thread_create("Super Duper", PRI_MAX, &task_1, NULL);
    //    thread_create("Burger King", PRI_MAX, &task_2, &tmp_tid);
    //    thread_create("In and Out", PRI_MAX, &task_3, &tmp_tid);
    thread_create("Task Shell", PRI_MAX, &task_shell, NULL);
}

static void task_1(void *param UNUSED) {
    int i = 0;
    while (i < 100) {
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

static void task_shell(void *param UNUSED) {
    run_shell();
}

