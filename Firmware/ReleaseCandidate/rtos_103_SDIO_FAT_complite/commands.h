#ifndef __COMMANDS_H
#define __COMMANDS_H

const unsigned char parameter_0[] = "player_id";  //
const unsigned char parameter_1[] = "team_id";  //
const unsigned char parameter_2[] = "shot_sound_file_name";
const unsigned char parameter_3[] = "hit_sound_file_name";
const unsigned char parameter_4[] = "friendly_fire";
const unsigned char parameter_5[] = "damage_index";//наносимый оружием урон
const unsigned char parameter_6[] = "bt_name";
const unsigned char parameter_7[] = "bt_bind_adress";
const unsigned char parameter_8[] = "bt_baud_rate";
const unsigned char parameter_9[] = "bt_role";
const unsigned char parameter_10[] = "bt_cmode";
const unsigned char parameter_11[] = "bt_pswd";
const unsigned char parameter_12[] = "empty_clip_sound_file_name";
const unsigned char parameter_13[] = "reload_sound_file_name";
const unsigned char parameter_14[] = "reload_time";
const unsigned char parameter_15[] = "rate";
const unsigned char parameter_16[] = "battary_full_voltage";
const unsigned char parameter_17[] = "battary_low_voltage";
const unsigned char parameter_18[] = "ir_power";
const unsigned char parameter_19[] = "start_game_sound_file_name";
const unsigned char parameter_20[] = "clips_after_start";//количество обойм после начала игры
const unsigned char parameter_21[] = "clips_capacity";//ёмкость обоймы
const unsigned char parameter_22[] = "autostart_game";//автоматически запускать игру после подочи питания
const unsigned char parameter_23[] = "backlight_level";//яркость фоновой подсветки датчиков
const unsigned char parameter_24[] = "ir_power_offset";//смещение на операционном усилителе
const unsigned char parameter_25[] = "sonar_sound_file_name";//звук сонара, будем воспроизводить пока не будет соединения по блютус
const unsigned char parameter_26[] = "health_after_start";//количество здоровья после старта
const unsigned char* parametersPointers[] =
{	parameter_0,
	parameter_1,
	parameter_2,
	parameter_3,
	parameter_4,
	parameter_5,
	parameter_6,
	parameter_7,
	parameter_8,
	parameter_9,
	parameter_10,
	parameter_11,
	parameter_12,
	parameter_13,
	parameter_14,
	parameter_15,
	parameter_16,
	parameter_17,
	parameter_18,
	parameter_19,
	parameter_20,
	parameter_21,
	parameter_22,
	parameter_23,
	parameter_24,
	parameter_25,
	parameter_26
};

const unsigned char at_name[] = "AT+NAME";  //
const unsigned char at_bind[] = "AT+BIND";
const unsigned char at_uart[] = "AT+UART";
const unsigned char at_role[] = "AT+ROLE";
const unsigned char at_cmode[] = "AT+CMODE";
const unsigned char at_pswd[] = "AT+PSWD";

#endif
