/*
 * examples.c
 *
 *  Created on: May 15, 2015
 *      Author: rreeves
 */
 
 #include "thread.h"
 
void test(void *params) {
	
	printf("\nTest start");
	
	int i;
	for (i = 0; i < 3; i++) {
		printf("\ntest %i", i);
		timer_msleep(2500000);   
	}

	printf("\nTest done\n");
}

static void loop(void *params) {
	int i;
	for (i = 0; i < 5; i++) {
		printf("\n%s: %i", (char *)params, i);
		timer_msleep(1000000);   
	}
}

void priority_ex(void *params) {
	
	printf("\nPriority start");
	
	tid_t id1 = thread_create("Max - 2 Priority", PRI_MAX - 2, &loop, "Max - 2");
	tid_t id2 = thread_create("Max - 1 Priority", PRI_MAX - 1, &loop, "Max - 1");
	tid_t id3 = thread_create("Max Priority", PRI_MAX, &loop, "Max");
	
	thread_wait(id1);
	thread_wait(id2);
	thread_wait(id3);
	
	printf("\nPriority end\n");
}
