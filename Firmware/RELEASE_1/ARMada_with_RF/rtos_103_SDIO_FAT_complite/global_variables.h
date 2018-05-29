#ifndef __GLOBAL_VARIABLES_H
#define __GLOBAL_VARIABLES_H
#include "stm32f10x.h"
#include "miles_protocol.h"
#include "types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//Глобальные переменные
/*************************************************************************/


#if DEVICE_ROLE==BANDANA
extern volatile bool bt_connect_hase_done;
extern volatile uint32_t bandana_game_session_ID;
#endif

extern volatile uint8_t tx_buffer[TX_BUFFER_SIZE]; 	//Буффер ИК-передатчика

#ifdef SENSORS_BECKLIGHT
extern volatile uint8_t sensors[16][4];//буфер для сдвиговых регистров датчиков
#else
extern volatile uint8_t sensors[4];//буфер для сдвиговых регистров датчиков
#endif

#ifdef DIRECTION_CALCULATION
extern volatile uint8_t hit_mask;
extern volatile uint8_t hit_timeout_counter;
extern xSemaphoreHandle xHitDirectionSemaphore;
#endif

extern volatile bool game_status;//статус указывает, в игре игрок, ли вне игры
extern volatile uint16_t ADC_values[4];//Буфер для значений ADC каналов
extern volatile tir_tx_buffer_cursor ir_tx_buffer_cursor; //курсор передающего ИК буфера
extern volatile TIM_TimeBaseInitTypeDef timer2;
extern volatile TIM_TimeBaseInitTypeDef timer3;
extern volatile TIM_TimeBaseInitTypeDef timer4;
extern volatile TIM_TimeBaseInitTypeDef timer5;
extern volatile TIM_TimeBaseInitTypeDef timer7;
extern volatile TIM_TimeBaseInitTypeDef timer8;
extern volatile TIM_OCInitTypeDef timer3_PWM;
extern volatile TIM_OCInitTypeDef timer8_PWM;
extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE]; 	//Буффер ИК-приемника
extern volatile uint8_t counter;
extern volatile uint16_t tim3_ir_rx_count;
extern volatile uint16_t tim2_ir_rx_count;
extern volatile uint16_t tim4_ir_rx_count;
extern volatile uint16_t tim5_ir_rx_count;
extern volatile uint8_t bandana_leds_on_counter;
extern volatile TRecStatus tim2_rec_state;
extern volatile TRecStatus tim3_rec_state;
extern volatile TRecStatus tim4_rec_state;
extern volatile TRecStatus tim5_rec_state;
//extern volatile tir_rx_bit TIM2_IR_RX_RAW_BUFFER[IR_RAW_BUFFER_SIZE];//буфер таймера 2 для "сырых" данных - длительности импулся и период в микросекундах
//extern volatile tir_rx_bit TIM3_IR_RX_RAW_BUFFER[IR_RAW_BUFFER_SIZE];//буфер таймера 3 для "сырых" данных - длительности импулся и период в микросекундах
//extern volatile tir_rx_bit TIM4_IR_RX_RAW_BUFFER[IR_RAW_BUFFER_SIZE];//буфер таймера 4 для "сырых" данных - длительности импулся и период в микросекундах
//extern volatile tir_rx_bit TIM5_IR_RX_RAW_BUFFER[IR_RAW_BUFFER_SIZE];//буфер таймера 5 для "сырых" данных - длительности импулся и период в микросекундах
extern volatile struct pressing_duration key_pressing_duration;//структура хранит счетчики длительности нажатия кнопок
extern volatile TKEYBOARD_EVENT  keyboard_event; //события клавиатуры
extern volatile TKEYBOARD_EVENT  reload_key_event; //события клавиши "Перезарядить"

extern volatile uint8_t zone1_rx_buffer[ZONE_RX_BUFFER_SIZE]; 	//Буффер ИК-приемника
extern volatile uint8_t zone2_rx_buffer[ZONE_RX_BUFFER_SIZE]; 	//Буффер ИК-приемника
extern volatile uint8_t zone3_rx_buffer[ZONE_RX_BUFFER_SIZE]; 	//Буффер ИК-приемника
extern volatile uint8_t zone4_rx_buffer[ZONE_RX_BUFFER_SIZE]; 	//Буффер ИК-приемника

extern volatile trx_event Zone1RxEvent;//события 1-ой зоны
extern volatile trx_event Zone2RxEvent;//события 2-ой зоны
extern volatile trx_event Zone3RxEvent;//события 3-ей зоны
extern volatile trx_event Zone4RxEvent;//события 4-ой зоны
//extern volatile bool wav_player_stop;//флаг, указывающий, что wav-плеер выключен, звук не воспроизводится
extern volatile bool hit_now_processing;//флаг, указывающий, что попадание обрабатывается уже
extern volatile uint8_t safe_counter; //счетчик времени спасения
extern volatile tarmada_system armadaSystem;//структура для хранения всех игровых параметров и настроек
extern volatile bool cr_received;//флаг, указывающий, что получен символ возврата корети - '\r'
extern volatile uint8_t countdown_shock_time;//обратный счетчик времени шока

extern xSemaphoreHandle xKeysSemaphore;
extern xSemaphoreHandle xSoundBuffHTSemaphore;
extern xSemaphoreHandle xSoundBuffTCSemaphore;
extern xSemaphoreHandle xZone1Semaphore;
extern xSemaphoreHandle xZone2Semaphore;
extern xSemaphoreHandle xZone3Semaphore;
extern xSemaphoreHandle xZone4Semaphore;
extern xSemaphoreHandle xZoneSemaphore;
extern xSemaphoreHandle xWavPlayerSemaphore;
extern xSemaphoreHandle xWavPlayerManagerSemaphore;
extern xSemaphoreHandle xPlayWavSoundSemaphore;
extern xSemaphoreHandle xHitSemaphore;
extern xSemaphoreHandle xUsart1Semaphore;
extern xSemaphoreHandle xBluetoothSemaphore;
extern xSemaphoreHandle xGameOverSemaphore;
extern xSemaphoreHandle xSDcardLockSemaphore;
extern xQueueHandle xEventQueue;

#ifdef SI4432_ENABLE
extern xSemaphoreHandle Si4432_IQR_Semaphore;
extern xSemaphoreHandle rf_tx_buff_Semaphore;
extern xQueueHandle xRfRxEventQueue;
extern xQueueHandle xRfRxUniquePackagesQueue;
#endif



#ifdef COLOR_LCD
extern volatile xSemaphoreHandle xColorLCDSemaphore;
extern volatile xSemaphoreHandle xColorLCD_DMA_Semaphore;
#endif

extern volatile bool screen_auto_refresh;
extern volatile tfire_mode last_fire_mode;
extern volatile tfire_mode curr_fire_mode;

extern volatile uint32_t request_text [4];
extern volatile bool bt_configured;
extern volatile ftime_t date;



#ifndef COLOR_LCD
#if DEVICE_ROLE==TAG
extern const char black_gun_image[504];
#endif
extern const char game_over_black_image[504];
#endif


#ifdef 	COLOR_LCD

#if DEVICE_ROLE==TAG
// Dimensions    : 128x128 pixels
// Size          : 32 768 Bytes
extern const uint16_t guns[0x4000];
#endif
//extern const uint16_t pingvin_pic[0x4000];
extern const uint16_t gerb_pic[0x4000];
extern const uint16_t heart_1[0xA50];
extern const uint16_t heart_2[0xA50];
extern const uint16_t heart_3[0xA50];
extern const uint16_t heart_4[0xA50];
/*
extern const uint16_t Batt_1[0x5C0];
extern const uint16_t Batt_2[0x5C0];
extern const uint16_t Batt_3[0x5C0];
extern const uint16_t Batt_4[0x5C0];
extern const uint16_t Batt_5[0x5C0];
*/
extern const uint16_t batt_long_level_0[0x500];//18x64
extern const uint16_t batt_long_level_1[0x500];//18x64
extern const uint16_t batt_long_level_2[0x500];//18x64
extern const uint16_t batt_long_level_3[0x500];//18x64
extern const uint16_t batt_long_level_4[0x500];//18x64
extern const uint16_t batt_long_level_5[0x500];//18x64
//extern const uint16_t batt_long_level_6[0x480];//18x64
//extern const uint16_t batt_long_level_7[0x480];//18x64
extern const uint16_t bullet[0x540]; //64x21
extern const uint16_t bluetooth1[0x200];//16x32 pixels
//extern const uint16_t clip[0x700];//28x64 pixels
//extern const uint16_t clip_1[0x700];//28x64 pixels
extern const uint16_t clip_2[0x8C0];//35x64 pixels
extern const uint16_t game_over_pic[0x4000];//128x128 pixels

#endif

#endif
