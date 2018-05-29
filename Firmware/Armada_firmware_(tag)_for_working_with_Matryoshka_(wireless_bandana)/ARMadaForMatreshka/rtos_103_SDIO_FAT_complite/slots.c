#include "global_variables.h"
#include <string.h>
#include "slots.h"


//void get_int_argument_value(uint8_t* var_adress, uint8_t min_val, uint8_t max_val);


/*
void byte_parametr_slot(char* record, uint8_t* pvar )
{
	char* cursor_pos;
	cursor_pos = memchr(record,'=',strlen(record));
	if (cursor_pos != NULL){
		get_int_argument_value(pvar,);
	}

}
*/
/*
void word_parametr_slot(char* record, uint16_t* pvar )
{

}
*/

void get_int_argument_value(char* record, uint8_t* var_adress, uint8_t min_val, uint8_t max_val)
{

bool param_not_empty = false;
volatile char ch_tmp;
volatile uint8_t result = 0;

char* cursor_pos;
	cursor_pos = memchr(record,'=',strlen(record));
	if (cursor_pos != NULL){
		cursor_pos++;
		while(*cursor_pos !='\r')

			{

				ch_tmp = *cursor_pos++;
				if (ch_tmp==' ') continue; //игнорируем пробелы
				if ((ch_tmp >= '0')&&(ch_tmp<= '9'))
				{
					result = result*10+char_to_int(ch_tmp);
					param_not_empty = true;
				}
				else
				{
//					USART_SendStrP(parameter_invalid_error);
					return;

					//USART_SendStr("ERROR\n\r");
					//return invalid;//недопустимый аргумент

				}
			}
		if (!param_not_empty)
			{
//				USART_SendStrP(parameter_empty_error);
				return;//пустой аргумент
			}

		if ((result>max_val)||(result<min_val))
			{
//				USART_SendStrP(parameter_out_of_range_error);
				return;//аргумент больше максимально допустимого значения
			}
		*var_adress =  result;
//		eeprom_write_byte(var_adress, result);
//		USART_SendStr("OK\r\n");


	}//[if]


}

void get_word_argument_value(char* record, uint16_t* var_adress, uint16_t min_val, uint16_t max_val)
{

bool param_not_empty = false;
volatile char ch_tmp;
volatile uint16_t result = 0;

char* cursor_pos;
	cursor_pos = memchr(record,'=',strlen(record));
	if (cursor_pos != NULL){
		cursor_pos++;
		while(*cursor_pos !='\r')

			{

				ch_tmp = *cursor_pos++;
				if (ch_tmp==' ') continue; //игнорируем пробелы
				if ((ch_tmp >= '0')&&(ch_tmp<= '9'))
				{
					result = result*10+char_to_int(ch_tmp);
					param_not_empty = true;
				}
				else
				{
//					USART_SendStrP(parameter_invalid_error);
					return;

					//USART_SendStr("ERROR\n\r");
					//return invalid;//недопустимый аргумент

				}
			}
		if (!param_not_empty)
			{
//				USART_SendStrP(parameter_empty_error);
				return;//пустой аргумент
			}

		if ((result>max_val)||(result<min_val))
			{
//				USART_SendStrP(parameter_out_of_range_error);
				return;//аргумент больше максимально допустимого значения
			}
		*var_adress =  result;
//		eeprom_write_byte(var_adress, result);
//		USART_SendStr("OK\r\n");


	}//[if]


}



void get_double_word_argument_value(char* record, uint32_t* var_adress, uint32_t min_val, uint32_t max_val)
{

bool param_not_empty = false;
volatile char ch_tmp;
volatile uint32_t result = 0;

char* cursor_pos;
	cursor_pos = memchr(record,'=',strlen(record));
	if (cursor_pos != NULL){
		cursor_pos++;
		while(*cursor_pos !='\r')

			{

				ch_tmp = *cursor_pos++;
				if (ch_tmp==' ') continue; //игнорируем пробелы
				if ((ch_tmp >= '0')&&(ch_tmp<= '9'))
				{
					result = result*10+char_to_int(ch_tmp);
					param_not_empty = true;
				}
				else
				{
//					USART_SendStrP(parameter_invalid_error);
					return;

					//USART_SendStr("ERROR\n\r");
					//return invalid;//недопустимый аргумент

				}
			}
		if (!param_not_empty)
			{
//				USART_SendStrP(parameter_empty_error);
				return;//пустой аргумент
			}

		if ((result>max_val)||(result<min_val))
			{
//				USART_SendStrP(parameter_out_of_range_error);
				return;//аргумент больше максимально допустимого значения
			}
		*var_adress =  result;
//		eeprom_write_byte(var_adress, result);
//		USART_SendStr("OK\r\n");


	}//[if]


}






uint8_t char_to_int(char c)
     {
       switch(c)
       {
         case '0': return 0;
         case '1': return 1;
         case '2': return 2;
         case '3': return 3;
         case '4': return 4;
         case '5': return 5;
         case '6': return 6;
         case '7': return 7;
         case '8': return 8;
         case '9': return 9;
         default : return 0x55;

       }
     }




void trim(char *s)
{
    // удаляем пробелы и табы с начала строки:
    int i=0,j;
    while((s[i]==' ')||(s[i]=='\t'))
    {
         i++;
    }
    if(i>0)
    {
         for(j=0; j < strlen(s); j++)
         {
             s[j]=s[j+i];
         }
         s[j]='\0';
    }

    // удаляем пробелы и табы с конца строки:
    i=strlen(s)-1;
    while((s[i]==' ')||(s[i]=='\t')||(s[i]=='\r')||(s[i]=='\n'))
    {
         i--;
    }
    if(i < (strlen(s)-1))
    {
         s[i+1]='\0';
    }
}

void trim_mac_adress(char *s)
{
    // удаляем двоеточия
	volatile char* cursor_pos;
	cursor_pos = memchr(s,':',strlen(s));
	while(cursor_pos)//пока будем находить двоеточия
	{
		while(*(cursor_pos))//пока не конец строки
		{
			*cursor_pos = *(cursor_pos+1);//сдвигаем символы
			cursor_pos++;

		}
		cursor_pos = memchr(s,':',strlen(s));
	}
}

void convert_mac_adress(char *s)//приводим mac-адрес к виду, пригодному для использования с AT-командой
{
	if (strlen(s)==12)
	{
		memmove(&s[5],&s[4],8);//сдвигаем вправо на одну позицию 8 последних символов
		s[4]=',';
		memmove(&s[8],&s[7],6);//сдвигаем вправо на одну позицию 6 последних символов
		s[7]=',';
	}

}
void get_string_argument_value(char* record, char* var_string)
{
	char* cursor_pos;
		cursor_pos = memchr(record,'=',strlen(record));
		if (cursor_pos != NULL){
			cursor_pos++;
			trim(cursor_pos);
			memcpy(var_string,cursor_pos,strlen(cursor_pos));
		}
}
