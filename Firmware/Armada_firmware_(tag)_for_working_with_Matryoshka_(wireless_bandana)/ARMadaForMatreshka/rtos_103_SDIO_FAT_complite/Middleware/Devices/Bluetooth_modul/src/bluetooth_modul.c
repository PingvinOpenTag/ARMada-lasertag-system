#include "bluetooth_modul.h"
#include "global_variables.h"
#include "types.h"
#include "display.h"

#if DEVICE_ROLE==BANDANA
extern void bt_tag_init_with_game_status(bool, ttag_init_command);
#endif

void bluetooth_module_on_connected(void){


#if DEVICE_ROLE == BANDANA
				if(bt_connect_hase_done == false)
				{	bandana_game_session_ID = (uint32_t) xTaskGetTickCount();
					bt_connect_hase_done = true;
				}
		    	bt_tag_init_with_game_status(game_status, NEW_CONNECT);
//		    	display_show_bluetooth_icon();

#endif
#ifdef DISPLAY_ENABLE
		    	if(chiter_detected){
		    		display_show_chiter_picter();
		    	}
		    	else if(game_status!=OUT_OF_GAME)
		    	{
		    		display_init_gui();
		    		xSemaphoreGive(xGameOverSemaphore);//מעהאול סולאפמנ
		    		//display_show_bluetooth_icon();
		    	}

		    	else
		    	{
		    		display_show_game_over_picture();
#ifdef COLOR_LCD
		    		display_show_bluetooth_icon();
#endif
		    	}




#endif

}


void bluetooth_module_on_disconnected(void){


	if(game_status!=OUT_OF_GAME){
#if DEVICE_ROLE==TAG
		if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE){
//		game_status=OUT_OF_GAME;
#ifdef DISPLAY_ENABLE
		display_clear_screen();

    	display_show_guns_picture();
#endif
		}
#else
#ifdef DISPLAY_ENABLE
	display_hide_bluetooth_icon();
#endif
#endif

	}

#if DEVICE_ROLE==TAG
#ifdef DISPLAY_ENABLE
	display_clear_screen();
	display_show_guns_picture();
#endif
#endif



}
