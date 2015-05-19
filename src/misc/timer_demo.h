/* 
 * File:   timer_demo.h
 * Author: Shuai Yuan <syuanivy@gmail.com>
 *
 * Created on May 18, 2015, 5:43 PM
 */

#ifndef TIMER_DEMO_H
#define	TIMER_DEMO_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*Tasks for Testing Non-busy Waiting*/
void init_busy_test(void *);
void init_nonbusy_test(void *);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_DEMO_H */

