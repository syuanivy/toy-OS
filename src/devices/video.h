/*
 * vga.h
 *
 *  Created on: Jan 11, 2015
 *      Author: jcyescas
 */

#ifndef DEVICES_VGA_H_
#define DEVICES_VGA_H_

/* Initializes the video. */
void video_init();

/* Prints the given character in the console. */
void video_putc (char);

/* Cleans the console (whole screen). */
void video_clean();

#endif /* DEVICES_VGA_H_ */
