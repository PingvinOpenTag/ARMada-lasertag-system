#include "scripting_engine.h"
#include "gun.h"
#include "ff.h"
#include <string.h>

extern void parsing_string(char* record);//анализ строки
extern void trim(char *s);//удал€ем ненужные символы

typedef void (*tPfunc)(void); //указатель на вункцию без параметров и результатов


const unsigned char lsl_command_0[] = "puch_trigger_button";  //емул€ци€ нажати€ на спусковой крючЄк
const unsigned char lsl_command_1[] = "puch_reload_button";  //емул€ци€ нажати€ на кнопку перезар€дки

const unsigned char* pointers_of_lsl_commands_strings[] =
{	lsl_command_0,
	lsl_command_1
};




const tPfunc HandlerFuncPointers []={
		gun_on_trigger_pressing,
		gun_on_reload_pressing
};


extern char readBuffer[256];

//extern FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc_lsl;         /* file objects */

bool get_command_from_lsl_file(void)//получение натроек из
{
	FRESULT res;
	 res = f_open( &fsrc_lsl , "0:/scripts/on_shot.lsl" ,/* FA_CREATE_NEW*/FA_READ /*| FA_WRITE*/);
	    if ( res == FR_OK )
	    {
	    	while (f_gets(&readBuffer[0],256,&fsrc_lsl)/*readln(&string_buffer[0])*/)
	    	{
//	    		parsing_string(&readBuffer[0]);
	    	}
	    	f_close(&fsrc_lsl);
	    	return true;
	    }
	    else   return false;
}


uint8_t get_lsl_command_index(char* record)//проверим, что за параметр
{
//	volatile char* cursor_pos;
	uint8_t cmd_str_len;
	uint8_t record_len;

	for (unsigned int index=0; index <(sizeof(pointers_of_lsl_commands_strings)/sizeof(char*)); index++)
	{
		cmd_str_len = strlen((const char*)pointers_of_lsl_commands_strings[index]);
		record_len = strlen((const char*)record);
		if (memmem(record,record_len,pointers_of_lsl_commands_strings[index],cmd_str_len)!='\0'/*NULL*/)
		{
			return index;
		}
	}
	return 0xff;
}


