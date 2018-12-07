#include "types.h"

#ifdef SI4432_ENABLE
#include "network_tasks.h"
#include "FreeRTOS.h"
#include "task.h"
#include "global_variables.h"
#include "rf_modul.h"
#include "si4432.h"
#include  "network_low_level.h"
#include "network_base_types.h"
#include "global_variables.h"


volatile tPackageTimings timings_to_send;

void rf_tx_interrogate_task(void *pvParameters){
	static unsigned char reg_tmp;

	xRfRxEventQueue = xQueueCreate( RF_RX_BUFFER_SIZE, sizeof(tPackage));
	xRfRxUniquePackagesQueue=xQueueCreate( 1/*RF_RX_BUFFER_SIZE*/, sizeof(tPackage));
	xTaskCreate( rf_rx_interrogate_task, ( signed char * ) "rf_rx_task", configMINIMAL_STACK_SIZE*2, NULL, 0,
				                        ( xTaskHandle * ) NULL);
	xTaskCreate( rf_manager_task, ( signed char * ) "rf_manager_task", configMINIMAL_STACK_SIZE/2, NULL, 0,
				                        ( xTaskHandle * ) NULL);
/*
	xTaskCreate( rf_manager_task, ( signed char * ) "rf_sender_task", configMINIMAL_STACK_SIZE/2, NULL, 0,
				                        ( xTaskHandle * ) NULL);
*/
	timings_to_send.infiniteResend=0;
	timings_to_send.timeout = (defaultTimeout/1000000)*TIC_FQR;
	timings_to_send.resendTime = (defaultResendTime*TIC_FQR)/1000000;
	timings_to_send.resendTimeDelta = (defaultResendTimeDelta*TIC_FQR)/1000000;

	rf_FlushTxBuf();
	RF_modul_Init_with_parameters(/*dataRate9600BPS_DEV45KHZ*/dataRate2400BPS_DEV36KHZ,power_11dBm);
	reg_tmp =  si4432_spi_readReg(Si4432_Reg_Device_Type_Code);
		if(reg_tmp==0x08){

			FLASH_LED_ON;
			vTaskDelay(30);
			FLASH_LED_OFF;
			vTaskDelay(30);
			FLASH_LED_ON;
			vTaskDelay(30);
			FLASH_LED_OFF;
		}
		else{
			FLASH_LED_ON;
			vTaskDelay(900);
			FLASH_LED_OFF;
		}


	for (;;) {
		RF_modul_send_next_package();


	}



}


void rf_rx_interrogate_task(void *pvParameters){


	for (;;) {
		RF_modul_read_packages();



	}
}


void rf_manager_task(void *pvParameters){
static volatile int i=0;
i++;
	for (;;) {
		rf_listen();
	}
}

/*
void rf_sender_task(void *pvParameters){
	for (;;) {

	}

}
*/
#endif
