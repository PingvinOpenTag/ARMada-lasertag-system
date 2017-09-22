#ifndef __SRIPTING_ENGINE_H
#define __SRIPTING_ENGINE_H
//#include "global_variables.h"
#include "types.h"


//extern const unsigned char* pointers_of_lsl_commands_strings[];

uint8_t get_lsl_command_index(char* record);
bool get_command_from_lsl_file(void);

#endif
