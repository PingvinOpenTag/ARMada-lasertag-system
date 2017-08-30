#ifndef __MENU_H
#define __MENU_H
#include "types.h"
#ifdef DISPLAY_ENABLE

#ifndef COLOR_LCD
#include "stm32f10x.h"
#include "f10x-pcd8544.h"
#else
#include "ILI9163.h"
#include "GFXC.h"
#endif

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "global_variables.h"




typedef struct symbol_size {
uint8_t width;
uint8_t height;
} tsymbol_size;

typedef struct xy_position {
	uint8_t X;
	uint8_t Y;
} txy_position;


typedef struct menu_item {
	tsymbol_size symbol_size;
	char text[15];
	txy_position position;
	void (*pfunc)(void);
} tmenu_item;

typedef struct menu_item_list {
	tmenu_item* list[5];
	uint8_t count;
} tmenu_item_list;


typedef struct menu {
	uint8_t height;
	char* header_text;
	tmenu_item_list* item_list;
	uint8_t curr_item;
	uint8_t exit_index;
} tmenu;





enum setting_var_type{
	_BYTE,
	_WORD,
	_LONG,
	_BOOL,
	_ENUM,
	_YEAR,
	_MONTH,
	_DAY,
	_HOUR,
	_MINUTE,
	_SECOND,
	_SETT_APPLY,
	_SETT_CANSEL
};
typedef enum setting_var_type tsetting_var_type;


typedef struct setting_item {
	tsetting_var_type var_type;
	tsymbol_size symbol_size;
	char text[15];
	txy_position position;
	uint32_t var_address;
	uint32_t max_value;
	uint32_t min_value;
	uint8_t digits;
	uint32_t new_value_address;
} tsetting_item;



typedef struct setting_item_list {
	tsetting_item* list[5];
	uint8_t count;
} tsetting_item_list;



typedef struct settings {
	uint8_t height;
	char* header_text;
	tsetting_item_list* item_list;
	uint8_t curr_item;
	uint8_t apply_index;
	uint8_t cansel_index;
} tsettings;

#ifndef COLOR_LCD
extern void clear_screen(void);
#else
extern void ClrScrn(void);
#endif



void test1(void);
void test2(void);
void test3(void);

void show_menu( tmenu* menu);
void show_settings_page(tsettings* settings);
void init_item_list(void);
void init_sett_list(void);
void init_clock_sett_list(void);
void init_ir_sett_list(void);
void add_menu_item(const tmenu_item_list*, const tmenu_item*);
void add_setting_item(const tsetting_item_list*, const tsetting_item*);
void show_main_menu(void);
char* int_to_str(uint8_t x, uint8_t digits);
char* long_int_to_str(uint16_t x, uint8_t digits);
void sett_list_reset(tsetting_item_list* sett_item_list);

#endif
#endif /*__MENU_H*/
