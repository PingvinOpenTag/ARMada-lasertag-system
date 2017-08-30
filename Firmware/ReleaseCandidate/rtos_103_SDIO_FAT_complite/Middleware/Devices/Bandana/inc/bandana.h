#ifndef __BANDANA_H
#define __BANDANA_H
#include "global_variables.h"
#include "types.h"
void bandana_set_sensor_color_vibro(uint8_t color, bool vibro_enable,TDamageZone zone);
void bandana_all_sensors_set_color_vibro(uint8_t color, bool vibro_enable);
void bandana_all_sensors_off(void);
#endif
