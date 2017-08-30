#include "gun.h"
#include "global_variables.h"
#include "types.h"
#include "display.h"
#include "hardware_init.h"


extern tfire_mode test_fire_mode_switch(void);
extern TKEYBOARD_EVENT test_keyboard(int adc_value);

void gun_on_trigger_pressing(void){

		if((countdown_shock_time)||(game_status==OUT_OF_GAME))//если врем€ шока не прошло
																				// или внеигры
																				//игнорируем нажатие
		return;


/*		{
			keyboard_event = no_key_pressing;
        break;
		}
*/
		switch(test_fire_mode_switch())
		{
			case SINGLE:
			{
				DAC->DHR8R2=armadaSystem.gun.ir_power+armadaSystem.gun.ir_power_offset;//100;

				//	data_tmp = BKP_ReadBackupRegister(BKP_DR1);
				//	 BKP_WriteBackupRegister(BKP_DR1, BKP_ReadBackupRegister(BKP_DR1)+1 );

   			if(armadaSystem.gun.rounds)
   			{
   				send_ir_shot_package();
  				armadaSystem.gun.rounds--;
#ifdef RTC_Enable
   			    BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif
#ifdef DISPLAY_ENABLE
if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//убедимс€, что мы точно в игре
{
	display_update_rounds();
	xSemaphoreGive(xGameOverSemaphore);
}
#endif
			armadaSystem.wav_player.type_of_sound_to_play = SHOT;
   				xSemaphoreGive(xWavPlayerManagerSemaphore);
   			}
   			else
   			{
  				armadaSystem.wav_player.type_of_sound_to_play = EMPTY_CLIP;
  				xSemaphoreGive(xWavPlayerManagerSemaphore);
   			}
			}
			break;
			case SEMI_AUTO:
			{
				DAC->DHR8R2=armadaSystem.gun.ir_power+armadaSystem.gun.ir_power_offset;//100;
				volatile TKEYBOARD_EVENT key_status;
//   			key_status = test_keyboard( ADC_values[0]);


				for (int i=3;i>0;i--)
	   			{
		   			key_status = test_keyboard( ADC_values[0]);
		   			if(key_status!=key_pressing) break;
					if(armadaSystem.gun.rounds)
				   	{


						send_ir_shot_package();
				   	    armadaSystem.gun.rounds--;
#ifdef RTC_Enable
		   			    BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif

#ifdef DISPLAY_ENABLE
if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//убедимс€, что мы точно в игре
{
#ifndef 	COLOR_LCD
lcd_rounds_update();
#else
color_lcd_rounds_update();
#endif
xSemaphoreGive(xGameOverSemaphore);
}
#endif



				  	    armadaSystem.wav_player.type_of_sound_to_play = SHOT;
				  	    xSemaphoreGive(xWavPlayerManagerSemaphore);
				   	    if(i>1)vTaskDelay((portTickType)(TIC_FQR*60)/armadaSystem.gun.rate);

				   	}
	   				else
				  	{
				  		armadaSystem.wav_player.type_of_sound_to_play = EMPTY_CLIP;
				  	    xSemaphoreGive(xWavPlayerManagerSemaphore);
//				  	    keyboard_event = no_key_pressing;
				  	    break;
				   	}
	   			}//[for]


			}
			break;
			case AUTO:
			{
				volatile TKEYBOARD_EVENT key_status;
   			key_status = test_keyboard( ADC_values[0]);
   			while (key_status==key_pressing)
   			{
   				if(game_status==OUT_OF_GAME)//если врем€ шока не прошло
   				   																				// или внеигры
   				   																				//игнорируем нажатие
   				   		{
//   				   			keyboard_event = no_key_pressing;
   				            break;
   				   		}

   				DAC->DHR8R2=armadaSystem.gun.ir_power+armadaSystem.gun.ir_power_offset;//100;
   				if(armadaSystem.gun.rounds)
   				{


   					send_ir_shot_package();
   					armadaSystem.gun.rounds--;
#ifdef RTC_Enable
	   			    BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif

#ifdef DISPLAY_ENABLE
if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//убедимс€, что мы точно в игре
{
#ifndef 	COLOR_LCD
lcd_rounds_update();
#else
color_lcd_rounds_update();
#endif
xSemaphoreGive(xGameOverSemaphore);
}
#endif


					armadaSystem.wav_player.type_of_sound_to_play = SHOT;
   					xSemaphoreGive(xWavPlayerManagerSemaphore);
   					vTaskDelay((portTickType)(TIC_FQR*60)/armadaSystem.gun.rate);
   					key_status = test_keyboard(ADC_values[0]);
   				}
				   	else
				   	{
				   		armadaSystem.wav_player.type_of_sound_to_play = EMPTY_CLIP;
				   	    xSemaphoreGive(xWavPlayerManagerSemaphore);
//				   	    keyboard_event = no_key_pressing;
				   	    break;
				   	}
				}   //[while]
			}


			break;
			default: break;



		} //[switch(test_fire_mode_switch())]


}



void gun_on_reload_pressing(void){
if(game_status==OUT_OF_GAME)//если врем€ шока не прошло
	   												// или внеигры
													//игнорируем нажатие
	{
		reload_key_event = no_key_pressing;
	    return;
	}

if(armadaSystem.gun.clips)
	{
	                   			armadaSystem.gun.rounds = armadaSystem.gun.rounds_placed_in_clip;
#ifdef RTC_Enable
	                   			BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif
	                   			armadaSystem.gun.clips--;
#ifdef RTC_Enable
	                   			BKP_WriteBackupRegister(BKP_DR2, armadaSystem.gun.clips);
#endif
	                   			armadaSystem.wav_player.type_of_sound_to_play = RELOAD;
	                   			xSemaphoreGive(xWavPlayerManagerSemaphore);
	                   			vTaskDelay((portTickType)armadaSystem.gun.reload_time*3);

#ifdef DISPLAY_ENABLE

	 if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//убедимс€, что мы точно в игре
		{
#ifndef 	COLOR_LCD
		lcd_rounds_update();
		lcd_clips_update();
	//	lcd_fire_mode_update(curr_fire_mode);
#else
		color_lcd_rounds_update();
		color_lcd_clips_update();
	//	color_lcd_fire_mode_update(curr_fire_mode);
#endif
		xSemaphoreGive(xGameOverSemaphore);
		}
#endif




		keyboard_event = no_key_pressing;//если во врем€ перезар€дки был нажат курк - сбросим это событие
	}

//	                   		reload_key_event = no_key_pressing;



}




/**************************************************************************************
* ‘ункц€ производит "выстрел"
* устанавлвает курсор на позицию начала блока данных data_packet.data[0]
* и разрешает передачу данных
* функци€ возвращает управление  только после отправки всех данных
***************************************************************************************/
void send_ir_shot_package(void){ //ќтправл€ем пакет ("стрел€ем")

	FLASH_LED_ON;
	ir_tx_buffer_cursor.byte_pos = 0;
	ir_tx_buffer_cursor.bit_mask = (1<<7);
	ir_tx_buffer_cursor.bits_for_tx=14;//"выстрел" состоит из 14 бит
	ir_tx_buffer_cursor.heder_been_sent = false;
	TIM_DeInit(TIM8);//сбрасываем настройки таймера на дефолтовые
//	TIM4->CCR1 = 50;
	init_pwm_TIM8();
}


