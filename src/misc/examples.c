/*
 * examples.c
 *
 *  Created on: May 15, 2015
 *      Author: rreeves
 */
 
 #include "thread.h"

void loop(void *params) {
	int i;
	for (i = 0; i < 5; i++) {
		printf("\n%s: %i", (char *)params, i);
		timer_msleep(1000000);   
	}
}

void priority_exec(void *params) {
	
	printf("\n===== Priority Execution Start =====\n");
	
	tid_t id1 = thread_create("p_default", PRI_DEFAULT, &loop, "default");	
	tid_t id2 = thread_create("p_max-1", PRI_MAX - 1, &loop, "max-1");
	tid_t id3 = thread_create("p_max", PRI_MAX, &loop, "max");
	tid_t id7 = thread_create("p_min", PRI_MIN, &loop, "min");
	
	thread_wait(id1);
	thread_wait(id2);
	thread_wait(id3);
	
	printf("\n===== Priority Execution End =====\n");
}

void priority_to_run(void *params) {
	
	tid_t id;
    int i = 0;
	for (i = 0; i < 20; i++) {
		printf("\nDefault: %i", i);
		timer_msleep(100);
		
		if (i == 10) {
			id = thread_create("p_max", PRI_MAX, &loop, "Max");
		}
	}
	
	thread_wait(id);
}

void priority_takeover(void *params) {
	
	printf("\n===== Priority Take Over Start =====\n");
	thread_wait(thread_create("ptakover_start", PRI_DEFAULT + 1, &priority_to_run, NULL));
	printf("\n===== Priority Take Over End =====\n");
}
