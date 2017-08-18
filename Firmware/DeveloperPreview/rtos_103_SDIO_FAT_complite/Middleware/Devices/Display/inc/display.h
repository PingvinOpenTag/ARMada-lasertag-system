#ifndef __DISPLAY_H
#define __DISPLAY_H
#include <stdio.h>
#include <stdint.h>

#include "cipher.h"
#include <GFXC.h>
#include "types.h"
#include "global_variables.h"

#ifndef 	COLOR_LCD
#define STANDARD_SYMBOL_HEIGHT 8
#define STANDARD_SYMBOL_WIDTH 6
#define HORIZONTAL_SCREEN_RESOLUTION 84
#define VERTICAL_SCREEN_RESOLUTION 48

#else
#define STANDARD_SYMBOL_HEIGHT 10
#define STANDARD_SYMBOL_WIDTH 6
#define HORIZONTAL_SCREEN_RESOLUTION 128
#define VERTICAL_SCREEN_RESOLUTION 128
#endif


void display_clear_screen(void);
void display_init(void);
// вывод строки
void display_put_string(unsigned char x, unsigned char y, const unsigned char str[]);

unsigned char display_standard_symbol_height(void);
unsigned char display_standard_symbol_width(void);
unsigned char display_horizontal_screen_resolution(void);
unsigned char display_vertical_screen_resolution(void);

#endif
