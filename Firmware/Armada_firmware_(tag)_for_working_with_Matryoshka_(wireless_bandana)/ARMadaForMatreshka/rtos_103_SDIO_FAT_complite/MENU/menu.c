#include "types.h"
#ifdef DISPLAY_ENABLE
#include "menu.h"
#include "stm32f10x_rtc.h"
#include "display.h"



#define symbol_width 6
#define symbol_height 8
#define header_height 8

extern uint32_t FtimeToCounter(ftime_t * ftime);
extern void CounterToFtime(uint32_t counter,ftime_t * ftime);
extern bool save_parameters_to_sd_card(void);
//extern void lcd8544_dma_refresh(void);

tmenu_item_list item_list;
tmenu main_menu;
tsetting_item_list sett_item_list;
tsettings sett_page;

const tmenu_item menu_item_1 = {
		{symbol_width,symbol_height},
		{"Calendar"},
		{symbol_width*1,header_height},
		test1
};

const tmenu_item menu_item_2 =  {
		{symbol_width,symbol_height},
		{"Clock"},
		{symbol_width*1,header_height+symbol_height*1},
		test2
};

const tmenu_item menu_item_3 =  {
		{symbol_width,symbol_height},
		{"IR Power"},
		{symbol_width*1,header_height+symbol_height*2},
		test3
};

const tmenu_item menu_item_4 =  {
		{symbol_width,symbol_height},
		{"EXIT"},
		{symbol_width*1,header_height+symbol_height*3},
		0
};

const char main_menu_header_text[] =  {"<< SETTINGS >>"};
const char sett_page_header_text[] =  {"<< Calendar >>"};
const char clock_page_header_text[] = {"<<  Clock  >>"};
const char ir_page_header_text[] =    {"<< IR power >>"};

volatile uint16_t new_ir_power_value;
const tsetting_item ir_setting_item_1 =  {
		_BYTE/*_WORD*/, //var type
		{symbol_width,symbol_height},//symbol size
		"Power",//text
		{symbol_width*1,header_height},// position (x,y)
		(uint32_t)&armadaSystem.gun.ir_power, // var adress
		(uint32_t) 100, //max value
		(uint32_t) 0, //min value
		(uint8_t) 3,//digits to display
		(uint32_t)&new_ir_power_value // new value address
};

volatile uint16_t new_ir_power_offset_value;
const tsetting_item ir_setting_item_2 =  {
		_BYTE/*_WORD*/, //var type
		{symbol_width,symbol_height},//symbol size
		"Offset",//text
		{symbol_width*1,header_height+symbol_height*1},// position (x,y)
		(uint32_t)&armadaSystem.gun.ir_power_offset, // var adress
		(uint32_t) 30, //max value
		(uint32_t) 0, //min value
		(uint8_t) 3,//digits to display
		(uint32_t)&new_ir_power_offset_value // new value address
};


volatile uint8_t new_day_value;
const tsetting_item setting_item_1 =  {
		_DAY, //var type
		{symbol_width,symbol_height},//symbol size
		"Day",//text
		{symbol_width*1,header_height},// position (x,y)
		(uint32_t)&date.day, // var adress
		(uint32_t) 31, //max value
		(uint32_t) 1, //min value
		(uint8_t) 2,//digits to display
		(uint32_t)&new_day_value // new value address
};

volatile uint8_t new_month_value;
const tsetting_item setting_item_2 =  {
		_MONTH, //var type
		{symbol_width,symbol_height},//symbol size
		"Month",//text
		{symbol_width*1,header_height+symbol_height*1},// position (x,y)
		(uint32_t)&date.month, // var adress
		(uint32_t) 12, //max value
		(uint32_t) 1, //min value
		(uint8_t) 2,//digits to display
		(uint32_t)&new_month_value // new value address
};

volatile uint16_t new_year_value;
const tsetting_item setting_item_3 =  {
		_YEAR, //var type
		{symbol_width,symbol_height},//symbol size
		"Year",//text
		{symbol_width*1,header_height+symbol_height*2},// position (x,y)
		(uint32_t)&date.year, // var adress
		(uint32_t) 2030, //max value
		(uint32_t) 2015, //min value
		(uint8_t) 4,//digits to display
		(uint32_t)&new_year_value // new value address
};



const tsetting_item setting_item_4 =  {
		_SETT_APPLY, //var type
		{symbol_width,symbol_height},//symbol size
		"APPLY",//text
		{symbol_width*1,header_height+symbol_height*3},// position (x,y)
		(uint32_t)0x00, // var adress
		(uint32_t) 12, //max value
		(uint32_t) 1, //min value
		(uint8_t) 2,//digits to display
		(uint32_t)0x00 // new value address
};

const tsetting_item setting_item_5 =  {
		_SETT_CANSEL, //var type
		{symbol_width,symbol_height},//symbol size
		"CANSEL",//text
		{symbol_width*1+7*symbol_width,header_height+symbol_height*3},// position (x,y)
		(uint32_t)0x00, // var adress
		(uint32_t) 12, //max value
		(uint32_t) 1, //min value
		(uint8_t) 2,//digits to display
		(uint32_t)0x00 // new value address
};


const tsetting_item setting_clock_hour_item = {
		_HOUR, //var type
		{symbol_width,symbol_height},//symbol size
		"Hour",//text
		{symbol_width*1,header_height},// position (x,y)
		(uint32_t)&date.hour, // var adress
		(uint32_t) 23, //max value
		(uint32_t) 0, //min value
		(uint8_t) 2,//digits to display
		(uint32_t)&new_day_value // чтобы не плодить сущности
};


const tsetting_item setting_clock_minute_item = {
		_MINUTE, //var type
		{symbol_width,symbol_height},//symbol size
		"Minute",//text
		{symbol_width*1,header_height+symbol_height*1},// position (x,y)
		(uint32_t)&date.minute, // var adress
		(uint32_t) 59, //max value
		(uint32_t) 0, //min value
		(uint8_t) 2,//digits to display
		(uint32_t)&new_month_value // чтобы не плодить сущности
};




void test1(void){
	 init_sett_list();
display_clear_screen();
//	clear_screen();
	show_settings_page((tsettings*) &sett_page);
#ifndef COLOR_LCD
	if (!screen_auto_refresh) lcd8544_dma_refresh();
#endif
}

void test2(void){
	init_clock_sett_list();
	display_clear_screen();
//	clear_screen();
	show_settings_page((tsettings*) &sett_page);
#ifndef COLOR_LCD
	if (!screen_auto_refresh) lcd8544_dma_refresh();
#endif
}

void test3(void){
//	volatile int i;
	init_ir_sett_list();
	display_clear_screen();
//	clear_screen();
	show_settings_page((tsettings*) &sett_page);
	save_parameters_to_sd_card();
#ifndef COLOR_LCD
	if (!screen_auto_refresh) lcd8544_dma_refresh();
#endif
//	i++;
}



void show_menu(tmenu* menu){
	bool exit = false;
#ifndef COLOR_LCD
	clear_screen();
	lcd8544_putstr(0, 0, (const unsigned char*)menu->header_text , 0); // вывод заголовка
#else
	volatile TextParamStruct TS;
	TS.Size = 0;
	TS.Font = StdFont;
	TS.XPos = 0;
	TS.YPos = 0;
	TS.TxtCol = iWhite;//iGreen;//iRed;
	TS.BkgCol =iBlack;//iWhite;
	TS.Precision = 2;
	TS.Padding = 2;

	if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
	 {
		 ClrScrn();
		 PStr(menu->header_text, (TextParamStruct*)&TS);
		 xSemaphoreGive(xColorLCDSemaphore);
	 }

#endif
	for(uint8_t i=0; i < menu->item_list->count; i++){
#ifndef COLOR_LCD
		lcd8544_putstr(menu->item_list->list[i]->position.X, menu->item_list->list[i]->position.Y,(const unsigned char*) menu->item_list->list[i]->text , 0); // вывод заголовка
#else
		TS.XPos = menu->item_list->list[i]->position.X;
		TS.YPos = menu->item_list->list[i]->position.Y;
		if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		{
			PStr(menu->item_list->list[i]->text, (TextParamStruct*)&TS);
			xSemaphoreGive(xColorLCDSemaphore);
		}
#endif
	}




	menu->curr_item = 0;
#ifndef COLOR_LCD
	lcd8544_putstr(menu->item_list->list[menu->curr_item]->position.X - symbol_width, menu->item_list->list[menu->curr_item]->position.Y, (const unsigned char*)">" , 0);
	if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#else
	TS.XPos = menu->item_list->list[menu->curr_item]->position.X - symbol_width;
	TS.YPos =menu->item_list->list[menu->curr_item]->position.Y;
	if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
			{
				PStr(">", (TextParamStruct*)&TS);
				xSemaphoreGive(xColorLCDSemaphore);
			}
#endif

	xSemaphoreTake( xKeysSemaphore, 30 );//очищаем симафор
	keyboard_event = no_key_pressing;
	reload_key_event = no_key_pressing;
	while (!exit){
		xSemaphoreTake( xKeysSemaphore, portMAX_DELAY );
			switch(reload_key_event){
		  			case no_key_pressing:
		  			{

		  			}
		  			break;
		  			case key_pressing:
		  			{
#ifndef COLOR_LCD
		  				fillRect(menu->item_list->list[menu->curr_item]->position.X - symbol_width, menu->item_list->list[menu->curr_item]->position.Y,symbol_width,symbol_height, 0);
#else
		  				TS.XPos = menu->item_list->list[menu->curr_item]->position.X - symbol_width;
		  				TS.YPos = menu->item_list->list[menu->curr_item]->position.Y;
		  				if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  				{
		  					PStr(" ",(TextParamStruct*) &TS);
		  					xSemaphoreGive(xColorLCDSemaphore);
		  				}

#endif
		  				if (menu->curr_item+1 < menu->item_list->count) {
		  					menu->curr_item +=1;

		  				}
		  				else if (menu->curr_item+1 == menu->item_list->count){
		  					menu->curr_item =0;

		  				}
#ifndef COLOR_LCD
		  				lcd8544_putstr(menu->item_list->list[menu->curr_item]->position.X - symbol_width, menu->item_list->list[menu->curr_item]->position.Y,(const unsigned char*) ">" , 0);
		  				if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#else
		  				TS.XPos = menu->item_list->list[menu->curr_item]->position.X - symbol_width;
		  				TS.YPos = menu->item_list->list[menu->curr_item]->position.Y;
		  				if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  				{
		  					PStr(">",(TextParamStruct*) &TS);
		  					xSemaphoreGive(xColorLCDSemaphore);
		  				}
#endif
		  				reload_key_event = no_key_pressing;
		  			}
		  			break;
		  	 		default:
		  	 		{
		  	 			reload_key_event = no_key_pressing;
		  	   		}
		  	 		break;
				}
			switch(keyboard_event){
		  			case no_key_pressing:
		  			{

		  			}
		  			break;
		  			case key_pressing:
		  			{
		  				if (menu->curr_item == menu->exit_index)
		  				{
		  					exit = true;
		  				}
		  				else {
		  					void (*pf)(void);
		  						pf = menu->item_list->list[menu->curr_item]->pfunc;
		  					if (pf)
		  					{
		  					 (*pf)();
		  					for(uint8_t i=0; i < menu->item_list->count; i++){

#ifndef COLOR_LCD
		  						lcd8544_putstr(menu->item_list->list[i]->position.X, menu->item_list->list[i]->position.Y,(const unsigned char*) menu->item_list->list[i]->text , 0); // вывод заголовка
#else
		  						TS.XPos = menu->item_list->list[i]->position.X;
		  						TS.YPos =  menu->item_list->list[i]->position.Y;
		  						if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  						{
		  							PStr(menu->item_list->list[i]->text, (TextParamStruct*)&TS);
		  							xSemaphoreGive(xColorLCDSemaphore);
		  						}
#endif

		  						}

		  					}
#ifndef COLOR_LCD
		  					clear_screen();
	  						lcd8544_putstr(0, 0, (const unsigned char*)menu->header_text , 0); // вывод заголовка
#else
	  						TS.XPos = 0;
	  						TS.YPos = 0;
	  						if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
	  						 {
	  							 ClrScrn();
	  							 PStr(menu->header_text, (TextParamStruct*)&TS);
	  							 xSemaphoreGive(xColorLCDSemaphore);
	  						 }

#endif
	  						for(uint8_t i=0; i < menu->item_list->count; i++){

#ifndef COLOR_LCD
	  							lcd8544_putstr(menu->item_list->list[i]->position.X, menu->item_list->list[i]->position.Y,(const unsigned char *) menu->item_list->list[i]->text , 0); // вывод заголовка
#else
	  							TS.XPos = menu->item_list->list[i]->position.X;
	  							TS.YPos = menu->item_list->list[i]->position.Y;
	  							if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
	  							{
	  								PStr(menu->item_list->list[i]->text,(TextParamStruct*) &TS);
	  								xSemaphoreGive(xColorLCDSemaphore);
	  							}
#endif

	  						}

#ifndef COLOR_LCD
	  						lcd8544_putstr(menu->item_list->list[menu->curr_item]->position.X - symbol_width, menu->item_list->list[menu->curr_item]->position.Y, (const unsigned char*) ">" , 0);
		  						if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#else
		  						TS.XPos = menu->item_list->list[menu->curr_item]->position.X - symbol_width;
		  						TS.YPos = menu->item_list->list[menu->curr_item]->position.Y;
		  						if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  						{
		  							PStr(">", (TextParamStruct*)&TS);
		  							xSemaphoreGive(xColorLCDSemaphore);
		  						}



#endif
		  				}

		  				keyboard_event = no_key_pressing;

		  			}
		  			break;
		  	 		default:
		  	 		{
		  	 			keyboard_event = no_key_pressing;
		  	   		}
		  	 		break;
				}


			}


#ifndef COLOR_LCD
			clear_screen();
#else
			if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
				 {
					 ClrScrn();
					 xSemaphoreGive(xColorLCDSemaphore);
				 }
#endif
}


enum setting_event{
	_NO_EVENT =0,
	_APPLY,
	_CANSEL,

};
typedef enum setting_var_type tsetting_var_type;

void show_settings_page(tsettings* settings){
	tsetting_var_type page_event = _NO_EVENT;
#ifndef COLOR_LCD
			clear_screen();
#else

	if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		 {
			 ClrScrn();
			 xSemaphoreGive(xColorLCDSemaphore);
		 }
#endif
	CounterToFtime ((uint32_t)RTC_GetCounter(),(ftime_t *)&date);
#ifndef COLOR_LCD
	lcd8544_putstr(0, 0, (const unsigned char*)settings->header_text , 0); // вывод заголовка
#else
	volatile TextParamStruct TS;
	TS.Size = 0;
	TS.Font = StdFont;
	TS.XPos = 0;
	TS.YPos = 0;
	TS.TxtCol = iWhite;//iGreen;//iRed;
	TS.BkgCol =iBlack;//iWhite;
	TS.Precision = 2;
	TS.Padding = 2;
		if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		{
			PStr(settings->header_text, (TextParamStruct*)&TS);
			xSemaphoreGive(xColorLCDSemaphore);
		}

#endif
	for(uint8_t i=0; i <settings->item_list->count; i++){

#ifndef COLOR_LCD
		lcd8544_putstr(settings->item_list->list[i]->position.X, settings->item_list->list[i]->position.Y,(const unsigned char*) settings->item_list->list[i]->text , 0);
#else
		TS.XPos = settings->item_list->list[i]->position.X;
		TS.YPos = settings->item_list->list[i]->position.Y;
		if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		{
			PStr(settings->item_list->list[i]->text,(TextParamStruct*) &TS);
			xSemaphoreGive(xColorLCDSemaphore);
		}

#endif
		switch(settings->item_list->list[i]->var_type){
				case _BYTE:
				{
					*((uint8_t*)settings->item_list->list[i]->new_value_address) = 	*((uint8_t*)settings->item_list->list[i]->var_address);
#ifndef COLOR_LCD
					lcd8544_putstr(settings->item_list->list[i]->position.X+7*symbol_width, settings->item_list->list[i]->position.Y,(const unsigned char*) int_to_str(*((uint8_t*)settings->item_list->list[i]->new_value_address),settings->item_list->list[i]->digits) , 0);
#else
		TS.XPos = settings->item_list->list[i]->position.X+7*symbol_width;
		TS.YPos = settings->item_list->list[i]->position.Y;
		if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		{
			PStr(int_to_str(*((uint8_t*)settings->item_list->list[i]->new_value_address),settings->item_list->list[i]->digits), (TextParamStruct*)&TS);
			xSemaphoreGive(xColorLCDSemaphore);
		}
#endif
				}
				break;
				case _WORD:
				{
					*((uint16_t*)settings->item_list->list[i]->new_value_address)= *((uint16_t*)settings->item_list->list[i]->var_address);
#ifndef COLOR_LCD
					lcd8544_putstr(settings->item_list->list[i]->position.X+7*symbol_width, settings->item_list->list[i]->position.Y, (const unsigned char*)long_int_to_str(*((uint16_t*)settings->item_list->list[i]->new_value_address),settings->item_list->list[i]->digits) , 0);
#else
					TS.XPos = settings->item_list->list[i]->position.X+7*symbol_width;
					TS.YPos = settings->item_list->list[i]->position.Y;
					if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
					{
						PStr(long_int_to_str(*((uint16_t*)settings->item_list->list[i]->new_value_address),settings->item_list->list[i]->digits), (TextParamStruct*)&TS);
						xSemaphoreGive(xColorLCDSemaphore);
					}
#endif
				}
				break;
				case _YEAR:
				{
					*((uint16_t*)settings->item_list->list[i]->new_value_address) = *((uint16_t*)settings->item_list->list[i]->var_address);

#ifndef COLOR_LCD
					lcd8544_putstr(settings->item_list->list[i]->position.X+7*symbol_width, settings->item_list->list[i]->position.Y, (const unsigned char*)long_int_to_str(*((uint16_t*)settings->item_list->list[i]->new_value_address),settings->item_list->list[i]->digits) , 0);
#else
					TS.XPos = settings->item_list->list[i]->position.X+7*symbol_width;
					TS.YPos = settings->item_list->list[i]->position.Y;
					if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
					{
						PStr(long_int_to_str(*((uint16_t*)settings->item_list->list[i]->new_value_address),settings->item_list->list[i]->digits), (TextParamStruct*)&TS);
						xSemaphoreGive(xColorLCDSemaphore);
					}

#endif

				}
				break;
				case _MONTH:
				{

				}
//				break;
				case _DAY:
				{

				}
//				break;
				case _HOUR:
				{

				}
//				break;
				case _MINUTE:
				{

				}
//				break;
				case _SECOND:
				{
					*((uint8_t*)settings->item_list->list[i]->new_value_address) = 	*((uint8_t*)settings->item_list->list[i]->var_address);

#ifndef COLOR_LCD
					lcd8544_putstr(settings->item_list->list[i]->position.X+7*symbol_width, settings->item_list->list[i]->position.Y, (const unsigned char*)int_to_str(*((uint8_t*)settings->item_list->list[i]->new_value_address),settings->item_list->list[i]->digits) , 0);
#else
					TS.XPos = settings->item_list->list[i]->position.X+7*symbol_width;
					TS.YPos =settings->item_list->list[i]->position.Y;
					if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
					{
						PStr(int_to_str(*((uint8_t*)settings->item_list->list[i]->new_value_address),settings->item_list->list[i]->digits),(TextParamStruct*) &TS);
						xSemaphoreGive(xColorLCDSemaphore);
					}


#endif
				}
				break;
				default:
				{

				}
				break;
			}




		}
#ifndef COLOR_LCD
	lcd8544_putstr(settings->item_list->list[settings->curr_item]->position.X - symbol_width, settings->item_list->list[settings->curr_item]->position.Y, (const unsigned char*)">" , 0);
	if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#else
	TS.XPos = settings->item_list->list[settings->curr_item]->position.X - symbol_width;
	TS.YPos = settings->item_list->list[settings->curr_item]->position.Y;
	if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
	{
		PStr( ">",(TextParamStruct*) &TS);
		xSemaphoreGive(xColorLCDSemaphore);
	}

#endif
	xSemaphoreTake( xKeysSemaphore, 30 );//очищаем симафор
	keyboard_event = no_key_pressing;
	reload_key_event = no_key_pressing;
	while (!page_event){
		xSemaphoreTake( xKeysSemaphore, portMAX_DELAY );
			switch(reload_key_event){
		  			case no_key_pressing:
		  			{

		  			}
		  			break;
		  			case key_pressing:
		  			{

#ifndef COLOR_LCD
		  				fillRect(settings->item_list->list[settings->curr_item]->position.X - symbol_width, settings->item_list->list[settings->curr_item]->position.Y,symbol_width,symbol_height, 0);
#else
		  				TS.XPos = settings->item_list->list[settings->curr_item]->position.X - symbol_width;
		  				TS.YPos = settings->item_list->list[settings->curr_item]->position.Y;
		  				if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  				{
		  					PStr(" ", (TextParamStruct*)&TS);
		  					xSemaphoreGive(xColorLCDSemaphore);
		  				}

#endif
		  				if (settings->curr_item+1 < settings->item_list->count) {
		  					settings->curr_item +=1;

		  				}
		  				else if (settings->curr_item+1 == settings->item_list->count){
		  					settings->curr_item =0;

		  				}

#ifndef COLOR_LCD
		  				lcd8544_putstr(settings->item_list->list[settings->curr_item]->position.X - symbol_width, settings->item_list->list[settings->curr_item]->position.Y, (const unsigned char*) ">" , 0);
		  				if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#else
		  				TS.XPos = settings->item_list->list[settings->curr_item]->position.X - symbol_width;
		  				TS.YPos = settings->item_list->list[settings->curr_item]->position.Y;
		  				if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  				{
		  					PStr(">", (TextParamStruct*)&TS);
		  					xSemaphoreGive(xColorLCDSemaphore);
		  				}

#endif

		  				reload_key_event = no_key_pressing;
		  			}
		  			break;
		  	 		default:
		  	 		{
		  	 			reload_key_event = no_key_pressing;
		  	   		}
		  	 		break;
				}
			switch(keyboard_event){
		  			case no_key_pressing:
		  			{

		  			}
		  			break;
		  			case key_pressing:
		  			{
		  				if (settings->curr_item == settings->apply_index)
		  				{
		  					for(uint8_t i=0; i <settings->item_list->count; i++){
		  						switch(settings->item_list->list[i]->var_type){
		  							case _WORD:
		  							{

		  							}
		  							//break;
		  							case _YEAR:
		  							{
		  								*((uint16_t*)settings->item_list->list[i]->var_address) = *((uint16_t*)settings->item_list->list[i]->new_value_address) ;
		  							}
		  							break;
		  							case _MONTH:
		  							{

		  							}
		  						//				break;
		  							case _DAY:
		  							{

		  							}
		  						//				break;
		  							case _HOUR:
		  							{

		  							}
		  						//				break;
		  							case _MINUTE:
		  							{

		  							}
		  						//				break;
		  							case _SECOND:
		  							{

		  							}
		  							case _BYTE:
		  							{
	  									*((uint8_t*)settings->item_list->list[i]->var_address)= *((uint8_t*)settings->item_list->list[i]->new_value_address) ;
		  							}
		  							break;
		  							default:
		  							{

		  							}
		  							break;
		  						}
		  					}
		  					RTC_SetCounter(FtimeToCounter((ftime_t *)&date));
		  					page_event = _APPLY;
		  					break;
		  				}

		  				else if (settings->curr_item == settings->cansel_index)
		  				{
		  					page_event = _CANSEL;
		  					break;
		  				}

		  				else{
#ifndef COLOR_LCD
		  					fillRect(settings->item_list->list[settings->curr_item]->position.X + symbol_width*7, settings->item_list->list[settings->curr_item]->position.Y,settings->item_list->list[settings->curr_item]->digits*symbol_width,symbol_height, 0);
#else
			  				TS.XPos = settings->item_list->list[settings->curr_item]->position.X + symbol_width*7;
			  				TS.YPos = settings->item_list->list[settings->curr_item]->position.Y;
			  				if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
			  				{
			  					PStr(" ",(TextParamStruct*) &TS);
			  					xSemaphoreGive(xColorLCDSemaphore);
			  				}
#endif
		  					switch(settings->item_list->list[settings->curr_item]->var_type){
		  									case _BYTE:
		  									{

		  										if (*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address) < ((uint8_t)settings->item_list->list[settings->curr_item]->max_value))
		  										{
		  											*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address) += 1;
		  										}
		  										else
		  										{
		  											*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address) = ((uint8_t)settings->item_list->list[settings->curr_item]->min_value);
		  										}

#ifndef COLOR_LCD
		  										lcd8544_putstr(settings->item_list->list[settings->curr_item]->position.X+7*symbol_width, settings->item_list->list[settings->curr_item]->position.Y, (const unsigned char*)int_to_str(*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address),settings->item_list->list[settings->curr_item]->digits) , 0);
#else
		  						  				TS.XPos = settings->item_list->list[settings->curr_item]->position.X+7*symbol_width;
		  						  				TS.YPos = settings->item_list->list[settings->curr_item]->position.Y;
		  						  				if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  						  				{
		  						  					PStr(int_to_str(*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address),settings->item_list->list[settings->curr_item]->digits),(TextParamStruct*) &TS);
		  						  					xSemaphoreGive(xColorLCDSemaphore);
		  						  				}
#endif

		  									}
		  									break;
		  									case _WORD:
		  									{
		  										//*((uint16_t*)settings->item_list->list[settings->curr_item]->new_value_address)= *((uint16_t*)settings->item_list->list[settings->curr_item]->var_address);
		  										//lcd8544_putstr(settings->item_list->list[settings->curr_item]->position.X+7*symbol_width, settings->item_list->list[settings->curr_item]->position.Y, long_int_to_str(*((uint16_t*)settings->item_list->list[settings->curr_item]->new_value_address),settings->item_list->list[settings->curr_item]->digits) , 0);

		  									}
		  									//break;
		  									case _YEAR:
		  									{
		  										if(*((uint16_t*)settings->item_list->list[settings->curr_item]->new_value_address) < (uint16_t)settings->item_list->list[settings->curr_item]->max_value)
		  										{

		  											*((uint16_t*)settings->item_list->list[settings->curr_item]->new_value_address) +=1;
		  										}
		  										else
		  										{
		  											*((uint16_t*)settings->item_list->list[settings->curr_item]->new_value_address) = (uint16_t)settings->item_list->list[settings->curr_item]->min_value;
												}


		  										//*((uint16_t*)settings->item_list->list[settings->curr_item]->new_value_address) = *((uint16_t*)settings->item_list->list[settings->curr_item]->var_address);

#ifndef COLOR_LCD
		  										lcd8544_putstr(settings->item_list->list[settings->curr_item]->position.X+7*symbol_width, settings->item_list->list[settings->curr_item]->position.Y,(const unsigned char*) long_int_to_str(*((uint16_t*)settings->item_list->list[settings->curr_item]->new_value_address),settings->item_list->list[settings->curr_item]->digits) , 0);
#else
		  						  				TS.XPos = settings->item_list->list[settings->curr_item]->position.X+7*symbol_width;
		  						  				TS.YPos = settings->item_list->list[settings->curr_item]->position.Y;
		  						  				if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  						  				{
		  						  					PStr( long_int_to_str(*((uint16_t*)settings->item_list->list[settings->curr_item]->new_value_address),settings->item_list->list[settings->curr_item]->digits),(TextParamStruct*) &TS);
		  						  					xSemaphoreGive(xColorLCDSemaphore);
		  						  				}
#endif

		  									}
		  									break;
		  									case _MONTH:
		  									{

		  									}
		  					//				break;
		  									case _DAY:
		  									{

		  									}
		  					//				break;
		  									case _HOUR:
		  									{

		  									}
		  					//				break;
		  									case _MINUTE:
		  									{

		  									}
		  					//				break;
		  									case _SECOND:
		  									{
		  										if (*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address) < (uint8_t)settings->item_list->list[settings->curr_item]->max_value)
		  										{
		  											*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address) += 1;
		  										}
		  										else
		  										{
		  											*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address) = ((uint8_t)settings->item_list->list[settings->curr_item]->min_value);
		  										}
#ifndef COLOR_LCD
		  										lcd8544_putstr(settings->item_list->list[settings->curr_item]->position.X+7*symbol_width, settings->item_list->list[settings->curr_item]->position.Y, (const unsigned char*)int_to_str(*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address),settings->item_list->list[settings->curr_item]->digits) , 0);
#else
		  						  				TS.XPos = settings->item_list->list[settings->curr_item]->position.X+7*symbol_width;
		  						  				TS.YPos = settings->item_list->list[settings->curr_item]->position.Y;
		  						  				if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		  						  				{
		  						  					PStr(int_to_str(*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address),settings->item_list->list[settings->curr_item]->digits), (TextParamStruct*)&TS);
		  						  					xSemaphoreGive(xColorLCDSemaphore);
		  						  				}
#endif


		  										//*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address) = 	*((uint8_t*)settings->item_list->list[settings->curr_item]->var_address);
		  										//lcd8544_putstr(settings->item_list->list[settings->curr_item]->position.X+7*symbol_width, settings->item_list->list[settings->curr_item]->position.Y, int_to_str(*((uint8_t*)settings->item_list->list[settings->curr_item]->new_value_address),settings->item_list->list[settings->curr_item]->digits) , 0);
		  									}
		  									break;
		  									default:
		  									{

		  									}
		  									break;
		  								}
#ifndef COLOR_LCD
		  					if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#endif
		  				}
		  				/*
		  				else {
		  					void (*pf)(void);
		  						pf = menu->item_list->list[menu->curr_item]->pfunc;
		  					if (pf)
		  					{
		  					 (*pf)();
		  					for(uint8_t i=0; i < menu->item_list->count; i++){
		  							lcd8544_putstr(menu->item_list->list[i]->position.X, menu->item_list->list[i]->position.Y, menu->item_list->list[i]->text , 0); // вывод заголовка
		  						}

		  					}
		  					clear_screen();
	  						lcd8544_putstr(0, 0, menu->header_text , 0); // вывод заголовка
	  						for(uint8_t i=0; i < menu->item_list->count; i++){
	  							lcd8544_putstr(menu->item_list->list[i]->position.X, menu->item_list->list[i]->position.Y, menu->item_list->list[i]->text , 0); // вывод заголовка
	  						}
		  					lcd8544_putstr(menu->item_list->list[menu->curr_item]->position.X - symbol_width, menu->item_list->list[menu->curr_item]->position.Y, ">" , 0);
		  						if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
		  				}
		  				*/
		  				keyboard_event = no_key_pressing;

		  			}
		  			break;
		  	 		default:
		  	 		{
		  	 			keyboard_event = no_key_pressing;
		  	   		}
		  	 		break;
				}


			}



}

void init_item_list(void){

	add_menu_item((tmenu_item_list*)&item_list, (tmenu_item*)&menu_item_1);
	add_menu_item((tmenu_item_list*)&item_list, (tmenu_item*)&menu_item_2);
	add_menu_item((tmenu_item_list*)&item_list, (tmenu_item*)&menu_item_3);
	add_menu_item((tmenu_item_list*)&item_list, (tmenu_item*)&menu_item_4);
	main_menu.header_text=(char*)main_menu_header_text;
	main_menu.height = header_height;
	main_menu.item_list = (tmenu_item_list*)&item_list;
	main_menu.curr_item = 0;
	main_menu.exit_index=3;



}



void init_sett_list(void){

	sett_list_reset((tsetting_item_list*)& sett_item_list);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_1);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_2);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_3);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_4);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_5);
	sett_page.header_text=(char*)sett_page_header_text;
	sett_page.height = header_height;
	sett_page.item_list = (tsetting_item_list*)& sett_item_list;
	sett_page.curr_item = 0;
	sett_page.apply_index =3;
	sett_page.cansel_index = 4;



}


void init_clock_sett_list(void){
	sett_list_reset((tsetting_item_list*)& sett_item_list);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_clock_hour_item);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_clock_minute_item);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_4);//apply item
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_5);//cansel item
	sett_page.header_text=(char*)clock_page_header_text;
	sett_page.height = header_height;
	sett_page.item_list = (tsetting_item_list*)& sett_item_list;
	sett_page.curr_item = 0;
	sett_page.apply_index =2;
	sett_page.cansel_index = 3;



}

void init_ir_sett_list(void){
	sett_list_reset((tsetting_item_list*)& sett_item_list);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&ir_setting_item_1);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&ir_setting_item_2);
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_4);//apply item
	add_setting_item((tsetting_item_list*)& sett_item_list, (tsetting_item*)&setting_item_5);//cansel item
	sett_page.header_text= (char*)ir_page_header_text;
	sett_page.height = header_height;
	sett_page.item_list = (tsetting_item_list*)& sett_item_list;
	sett_page.curr_item = 0;
	sett_page.apply_index =2;
	sett_page.cansel_index = 3;
}

void sett_list_reset(tsetting_item_list* sett_item_list){
	sett_item_list->list[0]=0;
	sett_item_list->list[1]=0;
	sett_item_list->list[2]=0;
	sett_item_list->list[3]=0;
	sett_item_list->list[4]=0;
	sett_item_list->list[5]=0;
	sett_item_list->count=0;
}


void add_menu_item(const tmenu_item_list* itm_list, const tmenu_item* menu_itm){
	tmenu_item_list* plist;
	plist = (tmenu_item_list*) itm_list;
	plist->list[plist->count]= (tmenu_item*) menu_itm;
	plist->count +=1;

}

void add_setting_item(const tsetting_item_list* itm_list, const tsetting_item* setting_itm){
	tsetting_item_list* plist;
	plist = (tsetting_item_list*) itm_list;
	plist->list[plist->count]= (tsetting_item*) setting_itm;
	plist->count +=1;
}
void show_main_menu(void){
	init_item_list();
	show_menu((tmenu*)&main_menu);

}


const char numbers[]={"0123456789"};

char* int_to_str(uint8_t x, uint8_t digits){

static volatile char str[6];



volatile uint8_t celoe, ostatok;
celoe=x;

int digits_tmp;
digits_tmp=digits;
if (digits == 0) digits_tmp=3;
      for (int i=(digits_tmp-1); i>=0; i--)
      {

      ostatok= celoe%10;
	  celoe  = celoe/10;
	  str[i]= numbers[ostatok];
      }
      str[digits_tmp]='\0';



if (digits == 0)
{
        while ((str[0]=='0')&&(str[1] !='\0')) for (int i=0; i < 6; i++) str[i]=str[i+1];
}

      return (char*) &str;

}

char* long_int_to_str(uint16_t x, uint8_t digits){

static volatile char str[6];



volatile uint16_t celoe, ostatok;
celoe=x;

int digits_tmp;
digits_tmp=digits;
if (digits == 0) digits_tmp=5;
      for (int i=(digits_tmp-1); i>=0; i--)
      {

      ostatok= celoe%10;
	  celoe  = celoe/10;
	  str[i]= numbers[ostatok];
      }
      str[digits_tmp]='\0';



if (digits == 0)
{
        while ((str[0]=='0')&&(str[1] !='\0')) for (int i=0; i < 6; i++) str[i]=str[i+1];
}

      return (char* )&str;

}
#endif
