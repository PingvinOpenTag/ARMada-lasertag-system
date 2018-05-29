#ifndef __DISPLAY_H
#define __DISPLAY_H


#include "cipher.h"
#include <GFXC.h>
#include "types.h"
#include "global_variables.h"
#include "f10x-pcd8544.h"

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

#define heart_x_pos 0
#define heart_y_pos 40

void display_clear_screen(void);
void display_init(void);
// вывод строки
void display_put_string(unsigned char x, unsigned char y, const unsigned char str[]);
void display_show_logo(void);
void display_show_guns_picture(void);
void display_show_bluetooth_icon(void);
void display_hide_bluetooth_icon(void);
void display_update_bluetooth_status(void);
void display_show_heart_picture(uint8_t picture_index);
void display_update_time(void);
void display_update_health(void);
void display_update_rounds(void);
void display_update_clips(void);
void display_update_battary_voltage(void);
void display_init_gui(void);
void display_update_fire_mode_status(void);
void display_backlight_off(void);
void display_backlight_on(void);
void display_show_game_over_picture(void);


unsigned char display_standard_symbol_height(void);
unsigned char display_standard_symbol_width(void);
unsigned char display_horizontal_screen_resolution(void);
unsigned char display_vertical_screen_resolution(void);

#endif
