/*
 * timer.h
 *
 *  Created on: Nov 8, 2014
 *      Author: jcyescas
 */

#ifndef DEVICES_TIMER_H_
#define DEVICES_TIMER_H_

void timer_init(void);

int timer_get_timestamp();

void timer_msleep(int milliseconds);

void timer_msleep_nonbusy(int microseconds);

void timer_wakeup();

#endif /* TIMER_H_ */
