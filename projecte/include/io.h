/*
 * io.h - Definició de l'entrada/sortida per pantalla en mode sistema
 */

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/** Screen functions **/
/**********************/

Byte inb (unsigned short port);
void printc(char c);
void printc_xy(Byte x, Byte y, char c);
void printk(char *string);
int setxy(int new_x, int new_y);
int set_col(int new_fg, int new_bg);

void zeos_show_performance(int diff);

#endif  /* __IO_H__ */
