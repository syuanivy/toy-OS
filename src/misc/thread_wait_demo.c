
#include <stdio.h>
#include "timer.h"
#include "thread.h"
#include "synch.h"
#include "thread_wait_demo.h"

static tid_t target_tid = 0;
static struct lock step_lock;
void thread_wait_demo(void *param UNUSED) {
    printf("\n===== Thread Wait Demo Initializing Start =====\n");
    lock_init(&step_lock);
    target_tid = thread_create("kdemo_target", PRI_MAX, &task_target, NULL);
    thread_create("kdemo_waiter1", PRI_MAX, &task_waiter1, &target_tid);
    thread_create("kdemo_waiter2", PRI_MAX, &task_waiter2, &target_tid);
}

void task_target(void *param UNUSED) {
    int i = 0;
    struct thread *t = thread_current();
    while (i < 10) {
        timer_msleep(SLEEP_INTERVAL);
        printf("\nI'm %s and I'm still working...\n", t->name);
        ++i;
    }
}

void task_waiter1(void *param UNUSED) {
    tid_t *target_tid = param;
    struct thread *t = thread_current();
    lock_acquire(&step_lock);
    printf("\nI'm %s and I need to wait for %d\n", t->name, *target_tid);
    lock_release(&step_lock);
    thread_wait(*target_tid);
}

void task_waiter2(void *param UNUSED) {
    tid_t *target_tid = param;
    struct thread *t = thread_current();
    lock_acquire(&step_lock);
    printf("I'm %s and I need to wait for %d\n", t->name, *target_tid);
    lock_release(&step_lock);
    thread_wait(*target_tid);
}
