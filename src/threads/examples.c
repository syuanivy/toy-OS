/*
 * examples.c
 *
 *  Created on: May 15, 2015
 *      Author: rreeves
 */
 
 void test(void *param) {
-    int i;
-    for (i = 0; i < 3; i++) {
-        printf("\ntest %i", i);
-        timer_msleep(2500000);   
-    }
-    
-    printf("\ntest done\n");
-}
