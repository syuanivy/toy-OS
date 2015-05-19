#include <stdio.h>

#include "synch.h"
#include "timer_demo.h"
#include "thread.h"

static struct lock lock_task;
static struct lock lock_task_busy;
static struct lock lock_task_nonbusy;

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

void init_busy_test(int delay) {
    lock_init(&lock_task_busy);

    thread_create("busy sleeper", PRI_MAX, &task_busy_sleeper, delay);
    thread_create("awake runner", PRI_MAX, &task_runner, NULL);
}

void init_nonbusy_test(int delay) {
    lock_init(&lock_task_nonbusy);

    thread_create("non busy sleeper", PRI_MAX, &task_nonbusy_sleeper, delay);
    thread_create("awake runner", PRI_MAX, &task_runner, NULL);
}

