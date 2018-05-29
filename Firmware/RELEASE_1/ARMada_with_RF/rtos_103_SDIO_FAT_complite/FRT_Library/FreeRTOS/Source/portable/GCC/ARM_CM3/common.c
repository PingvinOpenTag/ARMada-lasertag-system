#include "FreeRTOS.h"
#include "task.h"

extern char putChar(char);
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	/* This function will get called if a task overflows its stack.   If the
	parameters are corrupt then inspect pxCurrentTCB to find which was the
	offending task. */
	putChar('*'); putChar('S');putChar('O');putChar('F');putChar('*');
	//	( void ) pxTask;
//	( void ) pcTaskName;
	for( ;; );
}

