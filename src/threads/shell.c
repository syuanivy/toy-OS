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
 #include "examples.h"
 
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
     
    printf("\n%d, %s, %s, %u, %u", 
        t->tid, 
        strlen(t->name) > 0 ? t->name : "[No Name]",
        get_thread_status(t->status),
        get_total_runtime(t, time),
        get_total_alivetime(t, time)
    );
}

static void print_threads_status() {
    interrupts_disable();

    printf("\nThread ID, Name, Status, Total run time (ms), Total alive time (ms)");
    thread_foreach(&print_thread_status, NULL);

    interrupts_enable();
}

void run_command(char *command, bool block) {
    
    thread_func *func;
    void *param;
    int32_t priority = PRI_MAX;
    
    if (strcmp(command, "test") == 0) {
        func = &test;
    }
    else {
        printf("\nAvailable commands:");
        printf("\ntest - for debugging only");
        
        return;
    }
    
    if (block) {
        func(param);
    }
    else {
        thread_create(command, priority, func, param);
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

        while (inputc != 13) {
            uart_putc(inputc);
            input[index++] = inputc;

            inputc = uart_getc();
        }

        input[index] = '\0';

    if (strcmp(input, "help") == 0) {
        printf("\nts - thread status - show running threads and their run times");
        printf("\nrun <func> - launch a thread function and wait for its completion");
        printf("\nbg <func> - launch a command in the background");
        printf("\nshutdown - shutdown the operating system");
    }
    else if (strcmp(input, "ts") == 0) {
        print_threads_status();
    }
    else if (memcmp(input, "run ", RUNSIZE) == 0) {
        uint32_t command_size = INPUTSIZE - RUNSIZE;
        char command[command_size];
        strlcpy(command, &input[RUNSIZE], command_size);
        
        run_command(command, true);
    }
    else if (memcmp(input, "bg ", BGSIZE) == 0) {
        uint32_t command_size = INPUTSIZE - BGSIZE;
        char command[command_size];
        strlcpy(command, &input[BGSIZE], command_size);
        
        run_command(command, false);
    }
    else if (strcmp(input, "shutdown") == 0) {
        break;
    }
    else {
        printf("\nUnknown command. Enter 'help' for list of commands.");
    }
}

    printf("\nGoodbye from the osOS shell");
}
