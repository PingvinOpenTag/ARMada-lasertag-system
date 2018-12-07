#ifndef __ARMADA_H
#define __ARMADA_H




#include "stm32f10x.h"
#include "usart.h"
#include "bluetooth.h"
// Подключаем FreeRTOS
#include "FreeRTOS.h"
// из всех возможностей используем пока только переключение задач
#include "task.h"
#include "SDIO_SD/sdio_sd.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>
#include "types.h"
//#include "miles_protocol.h"
#include "semphr.h"
#include "queue.h"
#include "slots.h"
#include "global_variables.h"
#include "hardware_init.h"
#include "cipher.h"
#include "menu.h"
#include <GFXC.h>


#include "display.h"
#include "game.h"

#define HIT_PROCESSING_TIMEOUT (TIC_FQR/2) //полсекунды на обработку попаданий
#define USART_PROCESSING_TIMEOUT (TIC_FQR) //секунда на ожидание ответа

#define pi  3.14159
#define SHORT_DURATION 4 	//минимальная длительность (в "тиках") непрерывного нажатия курка,
							//необходимая для фиксации события "курок нажат"


typedef struct rx_hit{
portTickType time_stamp;
trx_packet rx_package;
} trx_hit;

typedef struct rx_captured_message{
portTickType time_stamp;
tir_message rx_message;
} trx_captured_message;

#define LCD_BL_OFF GPIO_SetBits(GPIOC, GPIO_Pin_13)//выключаем подсветку ЖКИ
#define LCD_BL_ON  GPIO_ResetBits(GPIOC, GPIO_Pin_13)//включаем подсветку ЖКИ

#define compareHits(hit1, hit2) ((((hit1.time_stamp-10) <= hit2.time_stamp) && ((hit1.time_stamp+10) >= hit2.time_stamp))&&(hit1.rx_package.player_id == hit2.rx_package.player_id)&&(hit1.rx_package.team_id == hit2.rx_package.team_id)&&(hit1.rx_package.damage == hit2.rx_package.damage))//может это тот же выстрел, но зафиксированный другой зоной?
#define compareMessages(mess1, mess2) ((mess1.time_stamp == mess2.time_stamp)&&(mess1.rx_message.ID == mess2.rx_message.ID)&&(mess1.rx_message.param == mess2.rx_message.param)&&(mess1.rx_message.control_byte == mess2.rx_message.control_byte))//может это тот же выстрел, но зафиксированный другой зоной?
//Дефалтовые значения игровых параметров, если не удастся считать конфиг-файл с SD-карты
#define DEFAULT_HEALTH 			100//0 			//уровень здоровья после
#define DEFAULT_TEAM_COLOR 		Yellow//Red			//цвет команды по умолчанию
#define DEFAULT_PLAYER_ID 		0
#define DEFAULT_GUN_DAMAGE 		Damage_50
#define DEFAULT_CLIPS 			15
#define DEFAULT_ROUNDS_IN_CLIP 	10

#define DEFAULT_PLAYER_STATUS 	IN_GAME//OUT_OF_GAME
#define DEFAULT_FRIENDLY_FIRE 	true//false
#define DEFAULT_SHOCK_TIME 	100//время шока по умолчанию - 1 с
#define DEFAULT_RELOAD_TIME 100//одна секунда время перезарядки по умолчанию
#define DEFAULT_RATE 300//скорострельность по умолчанию (выстрелов в минуту)
#define DEFAULT_BATTARY_FULL_VOLTAGE 4200 //напряжение полностью заряженной батареи по умолчанию
#define DEFAULT_BATTARY_LOW_VOLTAGE 3400 //напряжение полностью разряженной батареи по умолчанию
#define DEFAULT_IR_POWER 30 //мощность ИК излучения по умолчанию
#define DEFAULT_IR_POWER_OFFSET 0 //смещение на операционном усилителе
#define DEFAULT_AUTOSTART_GAME true //запускать игру после подачи питания
#define DEFAULT_BACKLIGHT_LEVEL 2


//Функции
/*************************************************************************/
void BluetoothTask(void *pvParameters);
void vTaskLED1(void *pvParameters);
void vTaskLED2(void *pvParameters);
void Wav_Player(void *pvParameters);
void Wav_Player_Manager(void *pvParameters);
void Zone4task(void *pvParameters);
void Zone3task(void *pvParameters);
void Zone2task(void *pvParameters);
void Zone1task(void *pvParameters);


#ifdef SI4432_ENABLE
void Si4432task(void *pvParameters);
//void Si4432_Init(void);
void Si4432_Rx_Tx_Init(void);
bool Si4432_TxBuf(unsigned char *pBuf, unsigned char bytes);
bool  Si4432_Rx(unsigned char *pBuf, unsigned char* pBytes, portTickType timeout);
#endif


#ifdef DIRECTION_CALCULATION
void DirectionCalculanionTask(void *pvParameters);
#endif
void init_gpio(void);
void init_timer(uint32_t sampleRate);//Настраиваем таймер
void init_dac(void);//Настраиваем ЦАП
void init_dma( uint16_t bitsPerSample);
void init_pwm(void);
void init_pwm_TIM3(void);
void init_pwm_TIM2(void);
void init_pwm_TIM4(void);
void init_pwm_TIM5(void);
void init_TIM7(void);
//void init_pwm_TIM8(void);
char wave_playback(const volatile char*);
tfire_mode test_fire_mode_switch(void);

void init_var(void);//присваиваем переменным значения по умолчанию
void ir_tx_cursor_home(void);//устанавливаем курсор в начало буфера

bool get_buffer_bit(uint8_t index);		//Считываем значение бита в буфере ИК-приемника

bool get_zone_buffer_bit(TDamageZone, uint8_t index);		//Считываем значение бита в буфере ИК-приемника
void set_buffer_bit(uint8_t, bool);	//Задаем значение биту в буфере ИК-приемника
void set_zone_buffer_bit (TDamageZone, uint8_t, bool);
void send_ir_shot_package(void); //Отправляем пакет ("стреляем")
void set_player_id(uint8_t);		//Задаем игроку идентификатор
void set_team_color(tteam_color);	//Задаем цвет нашей команды
void set_gun_damage(tgun_damage);	//Задаем мощьность нашего оружия (урон)
void init_adc(void);//настройка АЦП
void init_adc_for_multiscan(void);//настройка АЦП для последовательного сканирования каналов
void DMA_for_ADC_Init(void);//настройка DMA для работы с ADC
void SPISend(uint16_t);//отправка данных по SPI2
void init_spi2(void);//настройка SPI2
void init_dma_to_spi2(void);//настраиваем DMA для работы с SPI2 (датчиками)
void startSPI2(void);
bool stopWavPlayer(void);
void hit_processing(TDamageZone);//обрабатываем попадания;
void bt_hit_processing(trx_packet);//обрабатываем попадания, принятые по блютус
void message_processing(TDamageZone);//обрабатываем сообщения (команды пульта)
bool get_settings_from_ini_file(void);//получение натроек из  ini-файла
void parsing_string(char* record);//анализ строки
uint8_t get_parameter_index(char* record);//проверим, что за параметр
trx_packet get_packet_value(TDamageZone); //Считываем данные из полученного пакета

TKEYBOARD_STATUS get_keyboard_status(int adc_value); //Проверяем, нажат ли курок
TKEYBOARD_EVENT test_keyboard(int adc_value);//Проверяем события клавиатуры

TKEYBOARD_STATUS get_reload_key_status(int adc_value);//Проверяем, нажата ли клавиша "Перезарядить"
TKEYBOARD_EVENT test_reload_key(int adc_value);//Проверяем события клавиши "Перезарядить"
bool save_parameters_to_sd_card(void);
bool read_parameters_from_sd_card(void);
bool send_set_at_command(char* cmd, char* param);
bool send_test_at_command(void);
void send_package_by_bluetooth(trx_packet package);
void send_message_by_bluetooth(tir_message message);
void configure_bluetooth(void);//настраиваем блютус модуль
volatile  trx_hit hit_in_processing;//обрабатываемый в данный момент пакет
volatile trx_captured_message message_in_processing;
//char* int_to_str(uint8_t x, uint8_t digits);
//char* long_int_to_str(uint16_t x, uint8_t digits);
void bt_set_at_commands_mode(bool mode);//переводим блютус модуль в режим at-команд
void bt_reset(void);//аппаратный сброс блютус-модуля
uint8_t get_damage_index(uint8_t damage);
void copy_to_lcd_buffer(char* src);
void full_screen(char value);
void clear_screen(void);
tir_message get_ir_message_from_buffer(TDamageZone zone);
//void Sinus_calculate(void);

//bool get_bt_buffer_bit(uint8_t);	//Считываем значение бита в буфере пакета, принятого по блютус

void lock_firmware(void);
void BKPinit(void);

void RTC_Init(void);


uint32_t FtimeToCounter(ftime_t * ftime);
void CounterToFtime(uint32_t counter,ftime_t * ftime);
void getDateClockSettings (void);
void go_to_next_menu_item (uint8_t* item);
//void initGUI(void);//вывод на экран элементов пользовательского интерфейса
uint8_t checksum_for_clone_data(uint8_t*);
uint8_t checksum_for_tag_init_data(ttag_init_data);
//void bt_tag_init(void);
#if DEVICE_ROLE==BANDANA
void bt_tag_init_with_game_status(bool status, ttag_init_command);
#endif

#ifdef SENSORS_BECKLIGHT
void set_sensor_color(uint8_t color, uint8_t brightness_level, TDamageZone zone);
void set_sensor_vibro(uint8_t power_level, TDamageZone zone);
#endif





#endif /* __ARMADA_H */
