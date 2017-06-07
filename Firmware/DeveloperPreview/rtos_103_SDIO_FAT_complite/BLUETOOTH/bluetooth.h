#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "usart.h"



void set_bt_buffer_bit(uint8_t, bool);	//Задаем значение биту в буфере для ИК-пакета, принятого по блютус
trx_event parsing_bt_data(void);//анализируем пакет, полученный по блютус, извекаем из него данные
void test_bt_data(void);//обрабатываем приемный буфер блютус
bool get_bt_buffer_bit(uint8_t);	//Считываем значение бита в буфере пакета, принятого по блютус
trx_packet get_bt_packet_value(void); //Считываем данные из полученного пакета
tir_message get_bt_message_value(void); //Считываем сообщение из полученного пакета
//extern volatile uint8_t bt_rx_buffer[RX_BLUETOOTH_BUFFER_SIZE]; 	//Буффер для ИК-пакета, принятого по блютус

#endif /* __BLUETOOTH_H */
