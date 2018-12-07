#ifndef __TYPES_H
#define __TYPES_H

#include "stm32f10x.h"
#include "miles_protocol.h"

#define IN_GAME 				true
#define OUT_OF_GAME 			false

#define TAG 0
#define BANDANA 1


//если старая версия печатной платы
//#define OLD_VERSION_PCB

//роль устройства: BANDANA или TAG
//#define DEVICE_ROLE BANDANA
#define DEVICE_ROLE TAG


#define DISPLAY_ENABLE
//#define COLOR_LCD
#define LCD_NOT_STANDART


//#define RTC_Enable
#define LOW_DROP_ENABLE

//#define SI4432_ENABLE


//определение направления, откуда прилетел пакет
//#define DIRECTION_CALCULATION
#define HIT_TIMEOUT 10
// 1 to v1.1 , 2 to v2.2
#define LCD_MODUL_VERSION 2


#define batt_text_x_pos 80
#define batt_text_y_pos 21


#define FLASH_LED_ON (GPIO_SetBits(GPIOB, GPIO_Pin_1))
#define FLASH_LED_OFF (GPIO_ResetBits(GPIOB, GPIO_Pin_1))

#ifndef bool

                        #define bool unsigned char

                        #define true 1

                        #define false 0

#endif
//использовать фоновую подсветку датчиков
//#define SENSORS_BECKLIGHT



#define NUMBER_OF_SENSORS_FRAMES 8

#define TX_BUFFER_SIZE   40 //Размер буффера передатчика
#define RX_BUFFER_SIZE   40 //Размер буффера приемника
#define RX_BLUETOOTH_BUFFER_SIZE 40 //Размер буффера приемника (в байтах)
#define ZONE_RX_BUFFER_SIZE   40 //Размер буффера приемника зон поражения

#define IR_RAW_BUFFER_SIZE 256 //размер буфера ИК-приемника

#define SOUND_BUFFER_SIZE 512//512

#define HIGH_BRIDHTNESS (1<<4)
#ifdef SENSORS_BECKLIGHT
#define RED ((1<<2))//красный
#define BLUE ((1<<1))//синий
#define GREEN ((1<<0))//зелёный
#else
#define RED ((1<<2)|HIGH_BRIDHTNESS)//красный
#define BLUE ((1<<1)|HIGH_BRIDHTNESS)//синий
#define GREEN ((1<<0)|HIGH_BRIDHTNESS)//зелёный

#endif
#define CYAN (BLUE|GREEN)//голубой
#define YELLOW (RED|GREEN)//желтый
#define MAGENTA (RED|BLUE)//пурпурный
#define WHITE (RED|GREEN|BLUE)//белый
#define BLACK (0)
//((1<<4)|(1<<5)|(1<<6))//черный
#define VIBRO_HP (1<<3)//пин вибромоторчика (высокая мощность)
#define VIBRO_LP (1<<3)//пин вибромоторчика (малая мощность)
#define RED_WO_VIBRO RED
//(~((1<<6)| VIBRO_HP | VIBRO_LP))//красный без вибрации


/*
#define Z1 3;//индекс в массиве датчика первой зоны
#define Z2 2;//индекс в массиве датчика второй зоны
#define Z3 1;//индекс в массиве датчика третьей зоны
#define Z4 0;//индекс в массиве датчика четвертой зоны
*/
//--------------------------------------------------

typedef struct IR_Tx_Buffer_Cursor {
uint8_t byte_pos; //номен байта в буфере передатчика
uint8_t bit_mask;//битовая маска для получения текущего бита
uint16_t bits_for_tx;//сколько бит нужно ещё передать
bool heder_been_sent;//флаг, указывающий, отправлен ли заголовок пакета

} tir_tx_buffer_cursor;

typedef struct IR_RX_BIT {
uint16_t Pulse;
uint16_t Period;
} tir_rx_bit;





enum recStatus{
	REC_Idle,
	REC_Recording,
	REC_Captured
};
typedef enum recStatus TRecStatus;


enum damage_zone{
	zone_4,
	zone_3,
	zone_2,
	zone_1
};
typedef enum damage_zone TDamageZone;

struct pressing_duration
		{ uint16_t key_1      ; //длительность непрерывного
					    //нажатия кнопки 1 ("Клавиша огня")
		  unsigned int key_1_inc:1; //бит, разрешающий
					    //или запрещающий отсчет
					    //длительности нажатия кнопки 1
		  uint16_t key_2      ; //длительность непрерывного
					    //нажатия кнопки 2 ("Перезарядить")
		  unsigned int key_2_inc:1; //бит, разрешающий
					    //или запрещающий отсчет
					    //длительности нажатия кнопки 2



		  unsigned int no_key   ;

		};



enum typkeyboard_status {
			no_key_pressed,
			key_pressed
			};
typedef enum typkeyboard_status TKEYBOARD_STATUS;

enum typkeyboard_event  {
			no_key_pressing,
			key_pressing
	        };
typedef enum typkeyboard_event TKEYBOARD_EVENT;


//Определим перечисляемый тип для событий ИК-приемника
enum Rx_Event {
				NOT_EVENT, 		//нет событий
				RX_COMPLETE, 	//принят пакет
				RX_MESSAGE_COMPLETE,//принято сообщение
				RX_ERROR,		//ошибка приема пакета
				CHITER_DETECTED, //блютус-повязка выключалась во время игры
				BANDANA_BATTARY_LOW //на блютус-повязке разряжена батарея
			};
typedef enum Rx_Event trx_event; //Определим перечисляемый тип для событий ИК-приемника

//Опишем структуру, в которой будем хранить данные, считанные из полученного пакета
typedef struct RX_Packet {
uint8_t player_id;	//идентификатор попавшего в нас игрока
uint8_t team_id;	//идентификатор (цвет) его команды
uint8_t damage;		//приченённый нам урон
} trx_packet;

typedef struct tag_init_data_type{
	uint8_t player_id;
	uint8_t team_id;	//идентификатор (цвет) его команды
	bool game_status;
	uint8_t health;
	uint8_t command;
	uint32_t game_session_ID;//
	uint8_t Checksum;
} ttag_init_data;

//#pragma pack(push, 1)
typedef struct cloning_data_type{
/* Byte number*/
/*1*/	/*ID*/ 			//наименование сообщения
/*2*/	/*param*/			//параметр сообщения
/*3*/	/*control_byte*/	//контрольный байт
//непосредственно данные для клонирования
/*4*/	uint8_t reserved_1;		//зарезервировано
/*5*/	uint8_t reserved_2;		//зарезервировано
/*6*/	uint8_t reserved_3;		//зарезервировано
/*7*/	uint8_t Team_ID; 		//идентификатор команды
/*8*/	uint8_t reserved_4;		//зарезервировано
/*9*/ 	uint8_t Clips_added_by_picking_up_ammo_box;
/*10*/	uint8_t Health_added_by_picking_up_medic_box;
/*11*/ 	uint8_t reserved_5;		//зарезервировано
/*12*/ 	uint8_t Hit_LAD_timeout;//длительность вспышки светодиода при попадании, в секундах
/*13*/ 	uint8_t Sound_set;		//
/*14*/ 	uint8_t Overheat_limit;	//максимальное количество выстрелов в минуту, превышение которого приводит к перегреву ствола
/*15*/ 	uint8_t reserved_6;		//зарезервировано
/*16*/ 	uint8_t reserved_7;		//зарезервировано
/*17*/	uint8_t Damage_per_shot;//наносимый оружием урон за одно попадание
/*18*/	uint8_t Clip_size;		//ёмкость магазина, 0xFF - неограниченное количество
/*19*/	uint8_t Number_of_clips;//количество обойм, 0xFF - неограниченное количество
/*20*/	uint8_t Fire_selector;	//
/*21*/	uint8_t Nonber_of_rounds_of_burst_mode;//количество выстрелов при полуавтоматической стрельбе
/*22*/	uint8_t	Cyclic_RPM;
/*23*/ 	uint8_t	Reload_delay;	//время перезарядки, в секундах
/*24*/ 	uint8_t	IR_power;		//мощность ИК сигнала
/*25*/ 	uint8_t IR_range;		//
/*26*/ 	uint8_t Tagger_on_off_settings;		//
/*27*/ 	uint8_t Respawn_health;
/*28*/ 	uint8_t reserved_8;		//зарезервировано
/*29*/ 	uint8_t Respawn_delay_in_tens_of_seconds; //
/*30*/ 	uint8_t Armour_value;
/*31*/ 	uint8_t Game_on_off_settings_1_2;
/*32*/ 	uint8_t Game_on_off_settings_2_2;
/*33*/ 	uint8_t	Hit_delay;
/*34*/ 	uint8_t Start_delay_in_seconds; //в секундах
/*35*/ 	uint8_t Death_delay_in_seconds;//в секундах
/*36*/ 	uint8_t Time_limit_in_minutes;
/*37*/ 	uint8_t Maximum_respawns;
/*38*/ 	uint8_t reserved_9;		//зарезервировано
/*39*/ 	uint8_t Checksum;

} tcloning_date_package;

//#pragma pack(pop)

typedef union clone_data_union{
	tcloning_date_package clone_data;
	uint8_t raw_data[sizeof(tcloning_date_package)];
	ttag_init_data tag_init_data;
	uint8_t raw_tag_init_data[sizeof(ttag_init_data)];
} tclone_data_union;

//Определим структуру для хранения полученного по ИК сообщения

typedef struct IR_Message {
uint8_t ID; //наименование сообщения
uint8_t param;//параметр сообщения
uint8_t control_byte;//контрольный байт
tclone_data_union clone_data_union;//данные для клонирования
} tir_message;


typedef struct  player_type{
uint8_t id;	//идентификатор попавшего в нас игрока
uint8_t team_color;	//идентификатор (цвет) его команды
uint8_t health;		//текущее значение здоровья
uint8_t health_after_start;		//значение здоровья после старта игры
uint8_t shock_time;//время шока в 1/100 секунды
} tplayer;

typedef struct  pgun_type{
	tgun_damage damage; //наносимый урон
	uint16_t  rounds_placed_in_clip; //ёмкость обоймы
	uint16_t  rounds; //количество патронов в обойме в текущий момент
	uint16_t clips_after_start;//количество обойм после начала игры
	uint16_t clips; //количество оставшихся обойм
	uint16_t reload_time;//время перезарядки
	uint16_t rate;//скорострельность (выстрелов/мин)
	uint8_t ir_power;//мощность ИК излучения
	uint8_t ir_power_offset;//смещение на операционном усилителе
} tgun;



typedef struct battary_type{
	uint16_t full_voltage;//напряжение полностью заряженной батареи (в миллиВольтах)
	uint16_t low_voltage;//напряжение полностью разряженной батареи
} tbattary;

//Определим перечисляемый тип системных звуков
enum SystmSounds_Types {
	NOTHING,//молчим
	SHOT, 		//выстрел
	HIT, 	//ранение
	START_GAME,//начало новой игры
	GAME_OVER,//игрок вне игры
	EMPTY_CLIP,//пустая обойма
	RELOAD,//перезаряд
	MESSAGE,		//сообщение для игрока
	SONAR,//звук сонара
	USER_SOUND //для прочих звуков
	};
typedef enum SystmSounds_Types tsysSoundType; //Определим перечисляемый тип для событий ИК-приемника

//Определим перечисляемый тип для режимов огня
enum FireMode_Types {
	SINGLE,
	SEMI_AUTO,
	AUTO
};

typedef enum FireMode_Types tfire_mode;

typedef struct wav_player_type{
	bool stop;
	tsysSoundType type_of_reproduced_sound;//тип воспроизводимого в даныый момент звука
	tsysSoundType  type_of_sound_to_play;//тип звука, который нужно воспроизвести
	char shot_sound_file_name[32];
	char hit_sound_file_name[32];
	char start_game_sound_file_name[32];
	char game_over_sound_file_name[32];
	char empty_clip_sound_file_name[32];
	char reload_sound_file_name[32];
	char message_sound_file_name[32];
	char sonar_sound_file_name[32];
} twav_player;

//Определим источники ситемных событий
enum events_source_type {
	UNKNOWN,
	TRIGGER_KEY,
	RELOAD_KEY,
	FIRE_MODE_SWITCH,
	BLUETOOTH
};

typedef enum events_source_type tevents_source_type;

typedef struct system_event_type{
	tevents_source_type event_source;
	char event_code;
} tsystem_event_type;

/*
//Определим перечисляемый тип для режимов работы блютус
enum BT_Cmode_Types {
	connect_fixed_address = 0,//соединятся с конкретным устройством
	connect_any_address, 	//соединятся с любым устройством
	slave_loop //обратная петля
	};
typedef enum  BT_Cmode_Types BT_Cmode ; //Определим перечисляемый тип для событий ИК-приемника

//Определим перечисляемый тип для ролей блютус
enum BT_Role_Types {
	Slave = 0,//соединятся с конкретным устройством
	Master, 	//соединятся с любым устройством
	Slave_Loop //обратная петля
	};
typedef enum  BT_Role_Types BT_Role ; //Определим перечисляемый тип для событий ИК-приемника
*/



typedef struct bluetooth_type{
	uint32_t baudrate;//скорость обмена (битрейт)
	char name[32];//имя устойства в сети
	char mac_adress_for_bind[32];//mac-адрес устройства, к которому нужно автоматически подключится в режиме мастер, когда CMODE = 0 (connect fixed address)
	char cmode[4];
	char role[4];
	char pswd[6];
} tbluetooth;


typedef struct  system_type{
	tplayer player;//данные игрока
	tgun gun;//характеристики оружия
	twav_player wav_player;//характеристики и статус wav плеера
	bool friendly_fire;//дружественный огонь
	bool autostart_game;//запускать игру после включения питания
	tbluetooth bluetooth;//характеристики и настройки блютус модуля
	tbattary battary;//характеристики батареи
	uint8_t backlight_level;//уровень яркости фоновой подсветки
#if DEVICE_ROLE==TAG
	uint32_t game_session_ID;
#endif

} tarmada_system;






#define JD0 2451911 // дней до 01 янв 2001 ПН
typedef struct{
uint16_t year;
uint8_t month;
uint8_t day;
uint8_t hour;
uint8_t minute;
uint8_t second;
} ftime_t;


//Определим тип для команд инициализации тага
enum tag_init_command_type {
	NEW_CONNECT,
	NEW_GAME,
	STOP_GAME,
	HEALTH_UPDATE,
	PLAYER_TEAM_ID_UPDATE
};

typedef enum tag_init_command_type ttag_init_command;



enum picture_on_the_screen{
	NO_PICTURE,
	MAIN_GUI,
	GUNS_PIC,
	GAME_OVER_PIC,
	CHITER_PIC
};

typedef enum picture_on_the_screen tpicture_on_the_screen;


enum rf_data_type{
	PING,
	STATUS_DATA,
	NICKNAME
};

typedef enum rf_data_type trf_data_type;




#pragma pack(push, 1)

typedef struct  status_data_payload{
	 trf_data_type type; //тип передаваемых данных
	 uint8_t team_color :2;//цвет команды
	 uint8_t gun_damage:4;//урон, которое наносит оружие
	 uint8_t in_game :1;//в игре или вне игры
	 uint8_t bluetooth_is_connected:1;//подключена ли блютус-повязка
	 uint8_t player_id;//идентификатор игрока
	 uint8_t health;//здоровье
	 uint16_t battery_voltage;//напряжение на батарее (в миллиВольтах)
	 uint16_t clips;//количество обойм
	 unsigned char nickname[15];//никнейм
}  tstatus_payload;

#pragma pack(pop)


#endif /* __TYPES_H */
