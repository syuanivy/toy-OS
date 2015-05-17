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
		timer_msleep(2500000);   
	}
}

void priority_ex(void *params) {
	
	printf("\nPriority start");
	
	thread_create("Low Priority", PRI_DEFAULT - 1, &loop, "Low");
	thread_create("High Priority", PRI_DEFAULT + 1, &loop, "High");
	
	printf("\nPriority end\n");
}
