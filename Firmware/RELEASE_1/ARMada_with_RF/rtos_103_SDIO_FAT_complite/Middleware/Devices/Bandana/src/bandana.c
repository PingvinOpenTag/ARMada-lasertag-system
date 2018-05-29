#include "bandana.h"

void bandana_set_sensor_color_vibro(uint8_t color, bool vibro_enable,TDamageZone zone){
	uint8_t vibro_tmp=0;
#ifdef SENSORS_BECKLIGHT
	if(vibro_enable) vibro_tmp=NUMBER_OF_SENSORS_FRAMES;
	set_sensor_color(color|HIGH_BRIDHTNESS,vibro_tmp, zone);

#else
	if(vibro_enable) vibro_tmp=VIBRO_HP;
	sensors[zone]=color|vibro_tmp;

#endif

}


extern const uint8_t index_to_color[4];
void bandana_all_sensors_set_color_vibro(uint8_t color, bool vibro_enable){
	uint8_t vibro_tmp=0;
#ifdef SENSORS_BECKLIGHT
	if(vibro_enable) vibro_tmp=NUMBER_OF_SENSORS_FRAMES;
	  set_sensor_color(color|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_1);
	  set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_1);
	  set_sensor_color(color|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_2);
	  set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_2);
	  set_sensor_color(color|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_3);
	  set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_3);
	  set_sensor_color(color|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_4);
	  set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_4);
#else

	if(vibro_enable) vibro_tmp=VIBRO_HP;
	sensors[zone_1]=color|vibro_tmp;
	sensors[zone_2]=color|vibro_tmp;
	sensors[zone_3]=color|vibro_tmp;
	sensors[zone_4]=color|vibro_tmp;
#endif
}



void bandana_all_sensors_off(void){

#ifdef SENSORS_BECKLIGHT
	set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_1);
	 set_sensor_vibro(0,zone_1);
	 set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_2);
	 set_sensor_vibro(0,zone_2);
	 set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_3);
	 set_sensor_vibro(0,zone_3);
	 set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_4);
	 set_sensor_vibro(0,zone_4);
#else
	sensors[zone_1]=BLACK;
	sensors[zone_2]=BLACK;
	sensors[zone_3]=BLACK;
	sensors[zone_4]=BLACK;
#endif

}


