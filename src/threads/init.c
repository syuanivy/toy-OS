
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
static void init_shell();
static struct lock lock_task;

/*Tasks for Testing Non-busy Waiting*/
static void init_busy_test(int delay);
static void init_nonbusy_test(int delay);
static void task_busy_sleeper(void *);
static void task_nonbusy_sleeper(void *);
static void task_runner(void *);
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

    init_shell();

    while (true) {
        // never let idle thread run
        timer_msleep(1);
    }


    thread_exit();
}

static void init_shell() {
    thread_create("kshell", PRI_DEFAULT, &task_shell, NULL);
}

static void init_busy_test(int delay) {
    lock_init(&lock_task_busy);

    thread_create("busy sleeper", PRI_MAX, &task_busy_sleeper, delay);
    thread_create("awake runner", PRI_MAX, &task_runner, NULL);
}

static void init_nonbusy_test(int delay) {
    lock_init(&lock_task_nonbusy);

    thread_create("non busy sleeper", PRI_MAX, &task_nonbusy_sleeper, delay);
    thread_create("awake runner", PRI_MAX, &task_runner, NULL);
}

static void task_busy_sleeper(void *param UNUSED) {
  int delay = param;
  
  enum interrupts_level old_level = interrupts_disable();
  printf("\nI'm the busy sleeper and I will fall asleep for  %d microseconds\n", delay);
  interrupts_set_level(old_level);

  timer_msleep(delay);

  old_level = interrupts_disable();
  printf("\nI'm the busy sleeper and now I wake up after %d microseconds\n", delay);
  interrupts_set_level(old_level);

}

static void task_nonbusy_sleeper(void *param UNUSED) {
  int delay = param;

  enum interrupts_level old_level = interrupts_disable();
  printf("\nI'm the non busy sleeper and I will fall asleep for %d microseconds\n", delay);
  interrupts_set_level(old_level);

  timer_msleep_nonbusy(delay);

  old_level = interrupts_disable();
  printf("\nI'm the non busy sleeper and now I wake up after %d microseconds\n", delay);
  interrupts_set_level(old_level);
}

static void task_runner(void *param UNUSED) {
  int i = 0;

  int start = timer_get_timestamp();
  enum interrupts_level old_level = interrupts_disable();
  printf("\nI'm the awake runner and I start working at %d microseconds!\n", start);
  interrupts_set_level(old_level);
  
  while(i < 2500){
    i++;
    printf(".");
  }
  int end = timer_get_timestamp();

  printf("\nAwake runner has done its job at %d!\n", timer_get_timestamp());
  printf("\nTotal cost : %d microseconds!\n", end-start);
}