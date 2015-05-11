
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
#include "shell.h"

/* -ul: Maximum number of pages to put into palloc's user pool. */
static size_t user_page_limit = SIZE_MAX;

/* Tasks for the Threads. */
static void task_1(void *);
static void task_2(void *);
static void task_3(void *);
static void init_all_threads();
static struct lock lock_task;

/*Tasks for Testing Non-busy Waiting*/
static void init_busy_test();
static void init_nonbusy_test();
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

    init_nonbusy_test();

  //  run_shell();

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


static void init_busy_test() {
    lock_init(&lock_task_busy);
printf("\nentering init_busy_test, after lock_init");

    thread_create("busy sleeper", PRI_MAX, &task_busy_sleeper, NULL);
  //  thread_create("non busy sleeper", PRI_MAX, &task_nonbusy_sleeper, NULL);
    thread_create("awake runner", PRI_MAX, &task_runner, NULL);
}

static void init_nonbusy_test() {
    lock_init(&lock_task_nonbusy);
  //  thread_create("busy sleeper", PRI_MAX, &task_busy_sleeper, NULL);
    thread_create("non busy sleeper", PRI_MAX, &task_nonbusy_sleeper, NULL);
    thread_create("awake runner", PRI_MAX, &task_runner, NULL);
}

static void task_busy_sleeper(void *param UNUSED) {
  printf("\nI'm the busy sleeper and I will fall asleep for 2 seconds\n");
  timer_msleep(2000000);
  printf("\nI'm the busy sleeper and now I wake up\n");
}

static void task_nonbusy_sleeper(void *param UNUSED) {
  printf("\nI'm the non busy sleeper and I will fall asleep for 2 seconds\n");
  timer_msleep_nonbusy(200000);
  printf("\nI'm the non busy sleeper and now I wake up\n");
}

static void task_runner(void *param UNUSED) {
  printf("\nI'm the awake runner and I start working at %d microseconds!\n", timer_get_timestamp());
  int i = 0;
  while(i < 100){
    i++;
    printf("%d  ",i);
  }
  printf("\nAwake runner has done its job at %d!\n", timer_get_timestamp());
}



