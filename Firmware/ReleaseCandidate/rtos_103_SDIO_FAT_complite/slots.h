#ifndef __SLOTS_H
#define __SLOTS_H

//extern void byte_parametr_slot(char*,uint8_t*);
//extern void word_parametr_slot(char*,uint16_t*);
void get_int_argument_value(char* record, uint8_t* var_adress, uint8_t min_val, uint8_t max_val);
void get_word_argument_value(char* record, uint16_t* var_adress, uint16_t min_val, uint16_t max_val);
void get_double_word_argument_value(char* record, uint32_t* var_adress, uint32_t min_val, uint32_t max_val);
void get_string_argument_value(char* record, char* var_string);
uint8_t char_to_int(char c);
void trim(char *s);//удаляем ненужные символы
void trim_mac_adress(char *s);//удаляем ненужные символы из mac-адреса
void convert_mac_adress(char *s);//приводим mac-адрес к виду, пригодному для использования с AT-командой
#endif
