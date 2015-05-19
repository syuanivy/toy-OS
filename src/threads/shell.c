/*
 * shell.c
 *
 *  Created on: May 10, 2015
 *      Author: rreeves
 */

#include <stdio.h>
#include <string.h>
#include "thread.h"
#include "interrupt.h"
#include "shell.h"
#include "serial.h"
#include "timer.h"
#include "examples.h"
#include "thread_wait_demo.h"

void task_shell(void *param UNUSED) {
    run_shell();
}

static char* get_thread_status(enum thread_status status) {

    switch (status) {
        case THREAD_RUNNING:
            return "Running";
        case THREAD_READY:
            return "Ready";
        case THREAD_BLOCKED:
            return "Blocked";
        case THREAD_DYING:
            return "Dying";
        default:
            return "Undefined";
    }
 }
 
 static uint32_t get_total_runtime(struct thread *t, uint32_t time) {
     if (t->status == THREAD_RUNNING) {
         return (t->total_runtime + time - t->time_at_status) / 1000;
     }
     else {
         return t->total_runtime / 1000;
     }
 }
 
 static uint32_t get_total_alivetime(struct thread *t, uint32_t time) {
     return (time - t->start_time) / 1000;
 }
 
 static void print_thread_status(struct thread *t, void *param UNUSED) {
     
    uint32_t time = timer_get_timestamp();
    
    printf("\n%-15d\t%-15s\t%-15s\t%-15u\t%-15u\t%-15u",
            t->tid,
            strlen(t->name) > 0 ? t->name : "[No Name]",
            get_thread_status(t->status),
            t->priority,
            get_total_runtime(t, time),
            get_total_alivetime(t, time)
    );
}

static void print_threads_status() {
    enum interrupts_level old_level = interrupts_disable();

    printf("\n-----------------------------------------------------"
            "-------------------------------------");
    printf("\nTotal Thread(s): %u", thread_num_threads());
    printf("\nTotal Ready Thread(s): %u", thread_num_ready_threads());
    printf("\n-----------------------------------------------------"
            "-------------------------------------");
    printf("\n%-15s\t%-15s\t%-15s\t%-15s\t%-15s\t%-15s",
            "TID", "NAME", "STATUS", "PRIORITY", "RTIME(ms)", "ATIME(ms)");

    thread_foreach(&print_thread_status, NULL);
    
    interrupts_set_level(old_level);
    interrupts_enable();
}

void run_command(char *command, bool block) {
    
    thread_func *func;
    void *param;
    int32_t priority = PRI_DEFAULT;
    
    if (strcmp(command, "priority_exec") == 0) {
        func = &priority_exec;
        priority = PRI_MAX;
    }
    else if (strcmp(command, "priority_takeover") == 0) {
        func = &priority_takeover;
    }
    else if (strcmp(command, "thread_wait_demo") == 0) {
        func = &thread_wait_demo;
    }
    else if (strcmp(command, "loop") == 0) {
        func = &loop;
        param = "Loop";
    }
    else {
        printf("\nAvailable commands:");
        printf("\npriority_exec - demonstrates thread execution order based on priority");
        printf("\npriority_takeover - demonstrates a higher priority thread takes over if a lower priority thread is running");
        printf("\nthread_wait_demo - demonstrates thread waiting");
        printf("\nloop - loops and sleeps at default priority"); //demo non-busy sleep. run and then execute ts while it runs.
        
        return;
    } 
    
    uint32_t thread_id = thread_create(command, priority, func, param);
    
    if (block) {
        thread_wait(thread_id);
    }
}

void run_shell() {
    printf("\nStarting the osOS shell...\n");

    char input[INPUTSIZE];
    while (true) {

        memset(input, 0, INPUTSIZE);
        int index = 0;

        uart_puts("\nosO$ "); 

        char inputc = uart_getc();
        while (inputc != CR) {
            // TODO: navigation not implemented here...
            if (inputc == BS && index > 0) {
                // dealing with backspace, default behavior is just
                // going back and overprint the output, so we need 
                // to print sequence "\b \b" here.
                uart_write("\b \b", 3);
                index--;
            } else if (inputc != BS) {
                uart_putc(inputc);
                input[index++] = inputc;
            }
            inputc = uart_getc();
        }
        input[index] = '\0';

        if (strcmp(input, "help") == 0) {
            printf("\nts - thread status - show running threads and their run times");
            printf("\nrun <func> - launch a thread function and wait for its completion");
            printf("\nbg <func> - launch a command in the background");
            printf("\nshutdown - shutdown the operating system");
        } else if (strcmp(input, "ts") == 0) {
            print_threads_status();
        } else if (memcmp(input, "run ", RUNSIZE) == 0) {
            uint32_t command_size = INPUTSIZE - RUNSIZE;
            char command[command_size];
            strlcpy(command, &input[RUNSIZE], command_size);

            run_command(command, true);
        } else if (memcmp(input, "bg ", BGSIZE) == 0) {
            uint32_t command_size = INPUTSIZE - BGSIZE;
            char command[command_size];
            strlcpy(command, &input[BGSIZE], command_size);

            run_command(command, false);
        } else if (strcmp(input, "shutdown") == 0) {
            break;
        } else {
            printf("\nUnknown command. Enter 'help' for list of commands.");
        }
    }

    printf("\nGoodbye from the osOS shell");
}
