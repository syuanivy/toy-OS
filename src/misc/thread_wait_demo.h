/* 
 * File:   thread_wait_demo.h
 * Author: Hanzhou Shi <hanzhou87 at gmail.com>
 *
 * Created on May 17, 2015, 10:48 AM
 */

#ifndef THREAD_WAIT_DEMO_H
#define	THREAD_WAIT_DEMO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define SLEEP_INTERVAL 1000000//1s

void thread_wait_demo(void *param UNUSED);

void task_target(void *param UNUSED);

void task_waiter1(void *param UNUSED);

void task_waiter2(void *param UNUSED);

#ifdef	__cplusplus
}
#endif

#endif	/* THREAD_WAIT_DEMO_H */

