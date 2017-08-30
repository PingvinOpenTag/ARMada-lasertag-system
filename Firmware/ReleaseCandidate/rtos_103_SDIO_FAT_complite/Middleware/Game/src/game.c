#include "game.h"
#include "display.h"
#include "bandana.h"
#include <stdio.h>
#include <stdint.h>
#include "global_variables.h"

#if DEVICE_ROLE==BANDANA
extern void bt_tag_init_with_game_status(bool status, ttag_init_command command);
#endif



void game_over(void){
	//урон больше, чем остаток здоровья
						armadaSystem.player.health = 0;
						game_status = OUT_OF_GAME;
						armadaSystem.wav_player.type_of_sound_to_play = GAME_OVER;//надо воспроизвести звук "игра закончена"
	#if DEVICE_ROLE==BANDANA
						bt_tag_init_with_game_status(OUT_OF_GAME,STOP_GAME);
//	#elif DEVICE_ROLE==TAG

//	#else
//						send_package_by_bluetooth(hit.rx_package);
	#endif
#ifdef DISPLAY_ENABLE
						if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)
						{
							display_show_game_over_picture();
						}
#ifdef COLOR_LCD
						if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//если соединение установлено
				        {
							 display_show_bluetooth_icon();
				        }

#endif
#endif

					xSemaphoreGive(xWavPlayerManagerSemaphore);



}



void game_start_new(void){


	armadaSystem.player.health = armadaSystem.player.health_after_start;
#if DEVICE_ROLE == BANDANA
	bandana_game_session_ID = (uint32_t) xTaskGetTickCount();
	bt_tag_init_with_game_status(true, NEW_GAME);
#else
//	send_message_by_bluetooth(cap_message.rx_message);
#endif

	armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
	armadaSystem.gun.rounds = 0;
#ifdef DISPLAY_ENABLE
	display_init_gui();
#endif
	xSemaphoreGive(xGameOverSemaphore);//отдаем семафор
	  	  	  	armadaSystem.wav_player.type_of_sound_to_play = START_GAME;
	xSemaphoreGive(xWavPlayerManagerSemaphore);
	bandana_all_sensors_set_color_vibro(RED,false);
	vTaskDelay(TIC_FQR/3);
	bandana_all_sensors_set_color_vibro(BLUE,false);
	vTaskDelay(TIC_FQR/3);
	bandana_all_sensors_set_color_vibro(GREEN,false);
	vTaskDelay(TIC_FQR/3);


#ifdef SENSORS_BECKLIGHT
	bandana_all_sensors_set_color_vibro(BLACK,false);
	vTaskDelay(TIC_FQR/3);
	set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_1);
	set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_2);
	set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_3);
	set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_4);
#else

	bandana_all_sensors_set_color_vibro(BLACK,false);

#endif
	taskENTER_CRITICAL();
	game_status = true;
	taskEXIT_CRITICAL();
//									xSemaphoreGive(xGameOverSemaphore);//отдаем семафор
}
