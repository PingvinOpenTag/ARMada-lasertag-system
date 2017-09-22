#include "rf_modul.h"
#include "SI4432.h"
#include "network_base_types.h"
#include "global_variables.h"
#include  "network_low_level.h"

#ifdef SI4432_ENABLE
void si4432_IQR_callback(void){
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR( Si4432_IQR_Semaphore, &xHigherPriorityTaskWoken);

}


void RF_modul_Init_with_parameters(tRfSettings setting, tRF_Power tx_power){
	static unsigned char reg_tmp;
			xSemaphoreTake(Si4432_IQR_Semaphore, 1/*portMAX_DELAY*/ );
			init_spi_for_si4432();
			init_isr_for_si4432();
			//чтение статусов
			reg_tmp =  si4432_spi_readReg(Si4432_Reg_Interrupt_Status_1);
			reg_tmp =  si4432_spi_readReg(Si4432_Reg_Interrupt_Status_2);
			//выполнение программного сброса
			si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1, Si4432_Mode1_Soft_Reset);
			if (xSemaphoreTake(Si4432_IQR_Semaphore, 1500/*portMAX_DELAY*/ )== pdTRUE)//ждем 3 секунды прерывание Si4432
			  {//дождались
			 	 volatile int a;
			 	 a++;
			  }
			 else {// не дождались
				 volatile int b;
				 b++;
				 }

			//чтение статусов
			reg_tmp =  si4432_spi_readReg(Si4432_Reg_Interrupt_Status_1);
			reg_tmp =  si4432_spi_readReg(Si4432_Reg_Interrupt_Status_2);
			//установка центральной частоты 433,92 МГц
			si4432_spi_writeReg(Si4432_Reg_Frequency_Band_Select/*0x75*/, 0x53);
			si4432_spi_writeReg(Si4432_Reg_Nominal_Carrier_Frequency_H /*0x76*/, 0x62);
			si4432_spi_writeReg(Si4432_Reg_Nominal_Carrier_Frequency_L /*0x77*/, 0x00);
			RF_modul_SetRfParameters(setting);

//			si4432_spi_writeReg(Si4432_Reg_IF_FIlter_Bandwidth /*0x1C*/, 0x05); //write 0x05 to the IF Filter Bandwidth register
//				si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Oversamping_Rate/*0x20*/, 0xA1);//write 0xA1 to the Clock Recovery Oversampling Ratio register
//					si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_2 /*0x21*/, 0x20);	//write 0x20 to the Clock Recovery Offset 2 register
//					si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_1/*0x22*/, 0x4E); //write 0x4E to the Clock Recovery Offset 1 register
//					si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_0 /*0x23*/, 0xA5); //write 0xA5 to the Clock Recovery Offset 0 register
//					si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_1 /*0x24*/, 0x00);//write 0x00 to the Clock Recovery Timing Loop Gain 1 register
//					si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_0 /*0x25*/, 0x13);//write 0x13 to the Clock Recovery Timing Loop Gain 0 register
	//				si4432_spi_writeReg(Si4432_Reg_AFC_Loop_Gearshift_Override /*0x1D*/, 0x40);//write 0x40 to the AFC Loop Gearshift Override register
//					si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation/*0x72*/, 0x48);//write 0x1F to the Frequency Deviation register





			//set preamble detection threshold to 20bits

			si4432_spi_writeReg(Si4432_Reg_Preamble_Detection_Ctrl/*0x35*/, 0x28); //write 0x28 to the Preamble Detection Control  register
					//Disable header bytes; set variable packet length (the length of the payload is defined by the
					//received packet length field of the packet); set the synch word to two bytes long
					si4432_spi_writeReg(Si4432_Reg_Header_Ctrl_2 /*0x33*/, 0x02);//write 0x02 to the Header Control2 register
					//Set the sync word pattern to 0x2DD4
					si4432_spi_writeReg(Si4432_Reg_Synchronization_Word_3/*0x36*/, 0x2D);//write 0x2D to the Sync Word 3 register
					si4432_spi_writeReg(Si4432_Reg_Synchronization_Word_2/*0x37*/, 0xD4);//write 0xD4 to the Sync Word 2 registe
					//enable the TX & RX packet handler and CRC-16 (IBM) check
					si4432_spi_writeReg(Si4432_Reg_Data_Access_Ctrl/*0x30*/, 0x8D);//write 0x8D to the Data Access Control register
					//Disable the receive header filters
					si4432_spi_writeReg(Si4432_Reg_Header_Ctrl_1 /*0x32*/, 0x00 );//write 0x00 to the Header Control1 register
					//enable FIFO mode and GFSK modulation
					si4432_spi_writeReg(Si4432_Reg_Modulation_Mode_Ctrl_2 /*0x71*/, 0x63);//write 0x63 to the Modulation Mode Control 2 registe
			/*
					SpiWriteRegister(0x0C, 0x12);//write 0x12 to the GPIO1 Configuration(set the TX state)
					SpiWriteRegister(0x0D, 0x15);//write 0x15 to the GPIO2 Configuration(set the RX state)
			*/

					si4432_spi_writeReg(Si4432_Reg_GPIO_Config_0/*0x0B*/, 0x12);// настройка GPIO0 для включения режима передачи TX
					si4432_spi_writeReg(Si4432_Reg_GPIO_Config_1 /*0x0C*/, 0x15);//настройка GPIO1 для включения режима приема RX

					/*set the non-default Si4432 registers*/
					//set the VCO and PLL
					si4432_spi_writeReg(0x5A, 0x7F);//write 0x7F to the VCO Current Trimming register
					si4432_spi_writeReg(0x58, 0x80); //write 0x80 to the ChargepumpCurrentTrimmingOverride register
					si4432_spi_writeReg(0x59, 0x40);//write 0x40 to the Divider Current Trimming register
					//set the AGC
					si4432_spi_writeReg(0x6A, 0x0B);//write 0x0B to the AGC Override 2 register
					//set ADC reference voltage to 0.9V
					si4432_spi_writeReg(0x68, 0x04);//write 0x04 to the Deltasigma ADC Tuning 2 register
					si4432_spi_writeReg(0x1F, 0x03);//write 0x03 to the Clock Recovery Gearshift Override register
					//set  Crystal Oscillator Load Capacitance register
					si4432_spi_writeReg(0x09, 0xD7);	//write 0xD7 to the Crystal Oscillator Load Capacitance register

					/*
					 * 000 +1 dBm
					 * 001 +2 dBm
					 * 010 +5 dBm
					 * 011 +8 dBm
					 * 100 +11 dBm
					 * 101 +14 dBm
					 * 110 +17 dBm
					 * 111 +20 dBm
					 */

						//Настройка мощности передатчика 10 mW
					RF_modul_set_tx_power(tx_power);



}




void RF_modul_Init(void){

	static unsigned char reg_tmp;
		xSemaphoreTake(Si4432_IQR_Semaphore, 1/*portMAX_DELAY*/ );
		init_spi_for_si4432();
		init_isr_for_si4432();
		//чтение статусов
		reg_tmp =  si4432_spi_readReg(Si4432_Reg_Interrupt_Status_1);
		reg_tmp =  si4432_spi_readReg(Si4432_Reg_Interrupt_Status_2);
		//выполнение программного сброса
		si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1, Si4432_Mode1_Soft_Reset);
		if (xSemaphoreTake(Si4432_IQR_Semaphore, 1500/*portMAX_DELAY*/ )== pdTRUE)//ждем 3 секунды прерывание Si4432
		  {//дождались
		 	 volatile int a;
		 	 a++;
		  }
		 else {// не дождались
			 volatile int b;
			 b++;
			 }

		//чтение статусов
		reg_tmp =  si4432_spi_readReg(Si4432_Reg_Interrupt_Status_1);
		reg_tmp =  si4432_spi_readReg(Si4432_Reg_Interrupt_Status_2);
		//установка центральной частоты 433,92 МГц
		si4432_spi_writeReg(Si4432_Reg_Frequency_Band_Select/*0x75*/, 0x53);
		si4432_spi_writeReg(Si4432_Reg_Nominal_Carrier_Frequency_H /*0x76*/, 0x62);
		si4432_spi_writeReg(Si4432_Reg_Nominal_Carrier_Frequency_L /*0x77*/, 0x00);
		//установка скорости передачи (9.6 kbps)
		si4432_spi_writeReg(Si4432_Reg_Tx_Data_Rate_1/*0x6E*/, 0x4E);
		si4432_spi_writeReg(Si4432_Reg_Tx_Data_Rate_0 /*0x6F*/, 0xA5);
		si4432_spi_writeReg(Si4432_Reg_Modulation_Mode_Ctrl_1  /*0x70*/, 0x2C);

		/*set the modem parameters according to the excel calculator(parameters: 9.6 kbps, deviation:
		 45 kHz, channel filter BW:
		112.1 kHz*/

		si4432_spi_writeReg(Si4432_Reg_IF_FIlter_Bandwidth /*0x1C*/, 0x05); //write 0x05 to the IF Filter Bandwidth register
		si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Oversamping_Rate/*0x20*/, 0xA1);//write 0xA1 to the Clock Recovery Oversampling Ratio register
		si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_2 /*0x21*/, 0x20);	//write 0x20 to the Clock Recovery Offset 2 register
		si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_1/*0x22*/, 0x4E); //write 0x4E to the Clock Recovery Offset 1 register
		si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_0 /*0x23*/, 0xA5); //write 0xA5 to the Clock Recovery Offset 0 register
		si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_1 /*0x24*/, 0x00);//write 0x00 to the Clock Recovery Timing Loop Gain 1 register
		si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_0 /*0x25*/, 0x13);//write 0x13 to the Clock Recovery Timing Loop Gain 0 register
		si4432_spi_writeReg(Si4432_Reg_AFC_Loop_Gearshift_Override /*0x1D*/, 0x40);//write 0x40 to the AFC Loop Gearshift Override register
		si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation/*0x72*/, 0x48);//write 0x1F to the Frequency Deviation register
		//set preamble detection threshold to 20bits
		si4432_spi_writeReg(Si4432_Reg_Preamble_Detection_Ctrl/*0x35*/, 0x28); //write 0x28 to the Preamble Detection Control  register
		//Disable header bytes; set variable packet length (the length of the payload is defined by the
		//received packet length field of the packet); set the synch word to two bytes long
		si4432_spi_writeReg(Si4432_Reg_Header_Ctrl_2 /*0x33*/, 0x02);//write 0x02 to the Header Control2 register
		//Set the sync word pattern to 0x2DD4
		si4432_spi_writeReg(Si4432_Reg_Synchronization_Word_3/*0x36*/, 0x2D);//write 0x2D to the Sync Word 3 register
		si4432_spi_writeReg(Si4432_Reg_Synchronization_Word_2/*0x37*/, 0xD4);//write 0xD4 to the Sync Word 2 registe
		//enable the TX & RX packet handler and CRC-16 (IBM) check
		si4432_spi_writeReg(Si4432_Reg_Data_Access_Ctrl/*0x30*/, 0x8D);//write 0x8D to the Data Access Control register
		//Disable the receive header filters
		si4432_spi_writeReg(Si4432_Reg_Header_Ctrl_1 /*0x32*/, 0x00 );//write 0x00 to the Header Control1 register
		//enable FIFO mode and GFSK modulation
		si4432_spi_writeReg(Si4432_Reg_Modulation_Mode_Ctrl_2 /*0x71*/, 0x63);//write 0x63 to the Modulation Mode Control 2 registe
/*
		SpiWriteRegister(0x0C, 0x12);//write 0x12 to the GPIO1 Configuration(set the TX state)
		SpiWriteRegister(0x0D, 0x15);//write 0x15 to the GPIO2 Configuration(set the RX state)
*/

		si4432_spi_writeReg(Si4432_Reg_GPIO_Config_0/*0x0B*/, 0x12);// настройка GPIO0 для включения режима передачи TX
		si4432_spi_writeReg(Si4432_Reg_GPIO_Config_1 /*0x0C*/, 0x15);//настройка GPIO1 для включения режима приема RX

		/*set the non-default Si4432 registers*/
		//set the VCO and PLL
		si4432_spi_writeReg(0x5A, 0x7F);//write 0x7F to the VCO Current Trimming register
		si4432_spi_writeReg(0x58, 0x80); //write 0x80 to the ChargepumpCurrentTrimmingOverride register
		si4432_spi_writeReg(0x59, 0x40);//write 0x40 to the Divider Current Trimming register
		//set the AGC
		si4432_spi_writeReg(0x6A, 0x0B);//write 0x0B to the AGC Override 2 register
		//set ADC reference voltage to 0.9V
		si4432_spi_writeReg(0x68, 0x04);//write 0x04 to the Deltasigma ADC Tuning 2 register
		si4432_spi_writeReg(0x1F, 0x03);//write 0x03 to the Clock Recovery Gearshift Override register
		//set  Crystal Oscillator Load Capacitance register
		si4432_spi_writeReg(0x09, 0xD7);	//write 0xD7 to the Crystal Oscillator Load Capacitance register

		/*
		 * 000 +1 dBm
		 * 001 +2 dBm
		 * 010 +5 dBm
		 * 011 +8 dBm
		 * 100 +11 dBm
		 * 101 +14 dBm
		 * 110 +17 dBm
		 * 111 +20 dBm
		 */

			//Настройка мощности передатчика 10 mW
		RF_modul_set_tx_power(power_11dBm);
		/*
			reg_tmp =  si4432_spi_readReg(Si4432_Reg_TX_Power);
			reg_tmp &=(0b11111100);
			reg_tmp |=(0b00000000);
			si4432_spi_writeReg(Si4432_Reg_TX_Power, reg_tmp);
		 */

}



void RF_modul_set_tx_power(tRF_Power power_level){
	unsigned char reg_tmp;
	reg_tmp =  si4432_spi_readReg(Si4432_Reg_TX_Power);
	reg_tmp &=(0b11111000);
	reg_tmp |=(unsigned char)power_level;
	si4432_spi_writeReg(Si4432_Reg_TX_Power, reg_tmp);
}



bool RF_modul_TxBuf(unsigned char *pBuf, unsigned char bytes){
bool result = false;
//установка отклонения частоты (+-45 кГц) (необходимый параметр при GFSK модуляции)
//si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation /*0x72*/, 0x48);
	/*Настройка содержания пакета*/
	 //установка длины передаваемых данных - 8 байт
	si4432_spi_writeReg(Si4432_Reg_Packet_Length/*0x3E*/, bytes); //запись 8 в регистр длины передаваемых данных
	//заполнение FIFO передаваемыми данными
	si4432_spi_writeBuf(Si4432_Reg_FIFO_Access, pBuf, bytes);

	//for(i=0;i<bytes;i++)
	//	     si4432_spi_writeReg(Si4432_Reg_FIFO_Access/*0x7F*/, pBuf[i]);

	//Отключение всех прерываний кроме прерывания по отправке пакета
	 //Данное действие сделано для извещения МК о успешной отправке
	si4432_spi_writeReg(Si4432_Reg_Interrupt_Enable_1 /*0x05*/,Si4432_IT1_Packet_Sent /* 0x04*/);
	si4432_spi_writeReg(Si4432_Reg_Interrupt_Enable_2/*0x06*/, 0x00);
	 //Чтение регистров статусов прерываний, для очистки текущих прерываний и сброса NIRQ в лог. 1
	si4432_spi_readReg(Si4432_Reg_Interrupt_Status_1);
	si4432_spi_readReg(Si4432_Reg_Interrupt_Status_2);
	/*активация передатчика*/
	 //При этом радиомодуль формирует пакет и отправляет его автоматически
	si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x09);
	 //Ожидание прерывания по отправке
	if (xSemaphoreTake(Si4432_IQR_Semaphore, 1500/*portMAX_DELAY*/ )== pdTRUE)//ждем 3 секунды прерывание Si4432
	  {//дождались
	 	 result = true;
	  }
	 else {// не дождались
		 result = false;
		 }
	 //Чтение регистров статусов прерываний, для очистки текущих прерываний и сброса NIRQ в лог. 1
	si4432_spi_readReg(Si4432_Reg_Interrupt_Status_1);
	si4432_spi_readReg(Si4432_Reg_Interrupt_Status_2);
	return result;

}



bool  RF_modul_Rx(unsigned char *pBuf, unsigned char* pBytes, portTickType timeout){
bool result = false;
unsigned char ItStatus1,ItStatus2;
//установка отклонения частоты (+-45 кГц) (необходимый параметр при GFSK модуляции)
////////	si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation /*0x72*/, 0x48);
//si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation/*0x72*/, 0x1F);
//Включаем два прерывания:
//1) прерывание по приему валидного пакета;
//2) прерывание по ошибке CRC.
//	Активация приемника:
si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x05);//запись 0x05 в Operating Function Control 1 регистр

	si4432_spi_writeReg(Si4432_Reg_Interrupt_Enable_1/*0x05*/, Si4432_IT1_Valid_Packed_Received | Si4432_IT1_CRC_Error);
	si4432_spi_writeReg(Si4432_Reg_Interrupt_Enable_2/*0x06*/, /*Si4432_IT2_Valid_Preamble_Detected | */ /*Si4432_IT2_Invalid_Preamble_Detected*/ 0x00);
	 //Чтение регистров статусов прерываний, для очистки текущих прерываний и сброса NIRQ в лог. 1
	si4432_spi_readReg(Si4432_Reg_Interrupt_Status_1);
	si4432_spi_readReg(Si4432_Reg_Interrupt_Status_2);
	if (xSemaphoreTake(Si4432_IQR_Semaphore, timeout/*portMAX_DELAY*/ )== pdTRUE)//ждем 3 секунды прерывание Si4432
	  {//дождались
		//Чтение регистров статусов прерываний
		ItStatus1=si4432_spi_readReg(Si4432_Reg_Interrupt_Status_1);
		ItStatus2=si4432_spi_readReg(Si4432_Reg_Interrupt_Status_2);
/*
		if ((ItStatus2 & Si4432_IT2_Valid_Preamble_Detected) == Si4432_IT2_Valid_Preamble_Detected){
			volatile int m;
			m++;
		}
		if ((ItStatus2 & Si4432_IT2_Invalid_Preamble_Detected) == Si4432_IT2_Invalid_Preamble_Detected){
			volatile int k;
			k++;
		}
*/
		if((ItStatus1 & Si4432_IT1_CRC_Error) == Si4432_IT1_CRC_Error )//произошла ошибка CRC
		{
			 //отключение приёмника
			si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x01);//запись 0x01 в Operating Function Control 1 регистр
		     //сброс RX FIFO
			si4432_spi_writeReg(Si4432_Reg_Operating_Mode_2 /*0x08*/, 0x02);//запись 0x02 в Operating Function Control 2 регистр
			si4432_spi_writeReg(Si4432_Reg_Operating_Mode_2 /*0x08*/, 0x00);//запись 0x00 в Operating Function Control 2 регистр
			//включение приёмника
//			si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x05);//запись 0x05 в Operating Function Control 1 регистр
			result = false;
		}
		if((ItStatus1 & Si4432_IT1_Valid_Packed_Received) == Si4432_IT1_Valid_Packed_Received)////принят валидный пакет
		{
			 //отключение передатчика
			si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x01);//запись 0x01 в Operating Function Control 1 регистр
			//Чтение длины принятых данных
			*pBytes=si4432_spi_readReg(Si4432_Reg_Receive_Packet_Length/*0x4B*/);//чтение регистра Received Packet Length

//		for(i=0;i<*pBytes;i++)
//			pBuf[i]=si4432_spi_readReg(Si4432_Reg_FIFO_Access /*0x7F*/);//чтение FIFO Access регистра
			si4432_spi_readBuf(Si4432_Reg_FIFO_Access,pBuf,*pBytes);
		//сброс RX FIFO
		si4432_spi_writeReg(Si4432_Reg_Operating_Mode_2 /*0x08*/, 0x02);//запись 0x02 в Operating Function Control 2 регистр
		si4432_spi_writeReg(Si4432_Reg_Operating_Mode_2 /*0x08*/, 0x00);//запись 0x00 в Operating Function Control 2 регистр
		//включение приёмника
//		si4432_spi_writeReg(Si4432_Reg_Operating_Mode_1/*0x07*/, 0x05);//запись 0x05 в Operating Function Control 1 регистр
			result = true;

		}


	  }
	 else {// не дождались
		 result = false;
		 }


return result;

}

#define NMBR_OF_SAMPLE_SETTING1 0x07
#define NMBR_OF_PARAMETER 12
//The table store the precalculated modem parameters for different data rate cases.
//This table is used in the non compatible mode (for best RF performances)
//see the comments at the end of the line for more details.
uint8_t RMRfSettings[NMBR_OF_SAMPLE_SETTING1][NMBR_OF_PARAMETER] =
{
// IFBW,    COSR, 	CRO2, CRO1, CRO0, 	CTG1, CTG0, 	TDR1, TDR0, 	MMC1, 	FDEV, 	AFC
{  0x01,    0x83, 	0xc0, 0x13, 0xa9, 	0x00, 0x05, 	0x13, 0xa9, 	0x20, 	0x3a, 	0x40}, //DR: 2.4kbps, DEV: +-36kHz, BBBW: 75.2kHz
{  0x04,    0x41, 	0x60, 0x27, 0x52, 	0x00, 0x0a, 	0x27, 0x52, 	0x20, 	0x48, 	0x40}, //DR: 4.8kbps, DEV: +-45kHz, BBBW: 95.3kHz
{  0x91,    0x71, 	0x40, 0x34, 0x6e, 	0x00, 0x18, 	0x4e, 0xa5, 	0x20, 	0x48, 	0x40}, //DR: 9.6kbps, DEV: +-45kHz, BBBW: 112.8kHz
{  0x91,    0x39, 	0x20, 0x68, 0xDC, 	0x00, 0xD3, 	0x9D, 0x49, 	0x20, 	0x0F, 	0x40}, //DR: 19.2kbps, DEV: +-9.6kHz, BBBW: 28.8kHz
{  0x91, 	0x9C, 	0x00, 0xD1, 0xB7, 	0x01, 0xA6, 	0x09, 0xD5, 	0x00, 	0x1F, 	0x40}, //DR: 38.4kbps, DEV: +-19.2kHz, BBBW: 57.6kHz
{  0x92, 	0x68, 	0x01, 0x3A, 0x93, 	0x02, 0x78, 	0x0E, 0xBF, 	0x00, 	0x2E, 	0x40}, //DR: 57.6kbps, DEV: +-28.8kHz, BBBW: 86.4kHz
{  0x9B, 	0x34, 	0x02, 0x75, 0x25, 	0x04, 0xEE, 	0x1D, 0x7E, 	0x00, 	0x5C, 	0x00}, //DR: 115.2kbps, DEV: +-57.6kHz, BBBW: 172.8kHz

};

RF_ENUM RF_modul_SetRfParameters(tRfSettings setting){
	si4432_spi_writeReg(Si4432_Reg_IF_FIlter_Bandwidth,RMRfSettings[setting][0]);
	si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Oversamping_Rate, RMRfSettings[setting][1]);
	si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_2,RMRfSettings[setting][2]);
	si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_1, RMRfSettings[setting][3]);
	si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Offset_0, RMRfSettings[setting][4]);
	si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_1 , RMRfSettings[setting][5]);
	si4432_spi_writeReg(Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_0 , RMRfSettings[setting][6]);
	si4432_spi_writeReg(Si4432_Reg_Tx_Data_Rate_1, RMRfSettings[setting][7]);
	si4432_spi_writeReg(Si4432_Reg_Tx_Data_Rate_0, RMRfSettings[setting][8]);
	si4432_spi_writeReg(Si4432_Reg_Modulation_Mode_Ctrl_1, RMRfSettings[setting][9]);
	si4432_spi_writeReg(Si4432_Reg_Frequency_Deviation , RMRfSettings[setting][10]);
	si4432_spi_writeReg(Si4432_Reg_AFC_Loop_Gearshift_Override 	 , RMRfSettings[setting][11]);
	return RF_OK;
}





unsigned char RF_modul_get_RSSI(void){

	return  si4432_spi_readReg(Si4432_Reg_Received_Signal_Strength_Indicator );
}
/*

//The table store the precalculated modem parameters for different data rate cases.
//This table is used in the non compatible mode (for best RF performances)
//see the comments at the end of the line for more details.
code uint8 RMRfSettings[NMBR_OF_SAMPLE_SETTING1][NMBR_OF_PARAMETER] =
{
// IFBW, COSR, CRO2, CRO1, CRO0, CTG1, CTG0, TDR1, TDR0, MMC1, FDEV, AFC
{0x01, 0x83, 0xc0, 0x13, 0xa9, 0x00, 0x05, 0x13, 0xa9, 0x20, 0x3a, 0x40}, //DR: 2.4kbps, DEV: +-36kHz, BBBW: 75.2kHz
{0x04, 0x41, 0x60, 0x27, 0x52, 0x00, 0x0a, 0x27, 0x52, 0x20, 0x48, 0x40}, //DR: 4.8kbps, DEV: +-45kHz, BBBW: 95.3kHz
{0x91, 0x71, 0x40, 0x34, 0x6e, 0x00, 0x18, 0x4e, 0xa5, 0x20, 0x48, 0x40}, //DR: 9.6kbps, DEV: +-45kHz, BBBW: 112.8kHz
{0x91, 0x39, 0x20, 0x68, 0xDC, 0x00, 0xD3, 0x9D, 0x49, 0x20, 0x0F, 0x40}, //DR: 19.2kbps, DEV: +-9.6kHz, BBBW: 28.8kHz
{0x91, 0x9C, 0x00, 0xD1, 0xB7, 0x01, 0xA6, 0x09, 0xD5, 0x00, 0x1F, 0x40}, //DR: 38.4kbps, DEV: +-19.2kHz, BBBW: 57.6kHz
{0x92, 0x68, 0x01, 0x3A, 0x93, 0x02, 0x78, 0x0E, 0xBF, 0x00, 0x2E, 0x40}, //DR: 57.6kbps, DEV: +-28.8kHz, BBBW: 86.4kHz
{0x9B, 0x34, 0x02, 0x75, 0x25, 0x04, 0xEE, 0x1D, 0x7E, 0x00, 0x5C, 0x00}, //DR: 115.2kbps, DEV: +-57.6kHz, BBBW: 172.8kHz

};

---------------------------
*/

//unsigned char RF_RX_buffer_tmp [64];//


extern volatile tWaitingPackage rf_TxBuf[];

extern volatile unsigned char rf_txBufTail;
extern volatile unsigned char rf_txBufHead;
extern volatile unsigned char rf_txCount;



#define BLOCKING_PERIOD_AFTE_RECEIVING ((8000*TIC_FQR)/1000000)
#define BLOCKING_PERIOD_AFTE_TRANSMISSION (1000*TIC_FQR/1000000)

/*
#define BLOCKING_PERIOD_AFTE_RECEIVING 30*(TIC_FQR/300)
#define BLOCKING_PERIOD_AFTE_TRANSMISSION 10*(TIC_FQR/300)
*/

#define ONLY_RECIEVER

#ifdef  ONLY_RECIEVER
const tDeviceAddress target_rf_net_address = {1,1,1};
const tDeviceAddress my_rf_net_address = {1,1,2};
#else

const tDeviceAddress target_rf_net_address = {1,1,2};
const tDeviceAddress my_rf_net_address = {1,1,1};

#endif

extern volatile tPackageTimings timings_to_send;
/*
volatile int delivered_packages =0;
uint16_t delivered_packages_array[32];
volatile int translated_packages=0;
uint16_t translated_packages_array[32];
volatile int received_packages=0;
uint16_t received_packages_array[128];
*/
volatile int received_unique_packages=0;
uint16_t received_unique_packages_array[128];

//volatile int lost_packets=0;


void RF_modul_send_next_package(void){

	static volatile portTickType waiting_interval = portMAX_DELAY;
	static volatile portTickType unlock_time = 0;
	static volatile portTickType time_tmp;
	static volatile tPackage rx_package_tmp;
	unsigned char Bytes=0;
	static tAckPayload ackPayload_tmp;


//Ждем получения данных по радиоканалу
	if (RF_modul_Rx(&rx_package_tmp, (unsigned char*)&Bytes, waiting_interval)){
		//если данные получены
		FLASH_LED_ON;
		time_tmp =  xTaskGetTickCount();
		unlock_time = time_tmp + BLOCKING_PERIOD_AFTE_RECEIVING;
		waiting_interval = 	BLOCKING_PERIOD_AFTE_RECEIVING;
		if(!((rx_package_tmp.target.address[0]==my_rf_net_address.address[0])&&(rx_package_tmp.target.address[1]==my_rf_net_address.address[1])&&(rx_package_tmp.target.address[2]==my_rf_net_address.address[2])))return;//если адресованно не нам, выходим
		if (rx_package_tmp.details.needAck)//принятый пакет требует подтверждения доставки
				{
//					FLASH_LED_ON;
					ackPayload_tmp.payload_size=0x02;
					ackPayload_tmp.operationCode=acknoledgementCode;
					ackPayload_tmp.packageId=rx_package_tmp.details.packageId;
		//		rf_send(target_rf_net_address, my_rf_net_address, data_to_send,6,false, timings_to_send);
					rf_send(rx_package_tmp.sender, my_rf_net_address, (uint8_t*)&ackPayload_tmp,sizeof(tAckPayload),false, timings_to_send);
				}
				else//принятый пакет не требует подтверждения доставки
				{
					if ((rx_package_tmp.payload[0]==0x02)&&(rx_package_tmp.payload[1]==0xFF)&&(rx_package_tmp.payload[2]==0xFF))//если пришло подтверждение доставки
					{
						display_backlight_on();
						uint16_t pack_id_tmp;
						tAckPayload* pAckPayload_tmp;
						pAckPayload_tmp = (tAckPayload*)&rx_package_tmp.payload;
						pack_id_tmp = pAckPayload_tmp->packageId;
						rf_set_delivery_complite_flag(pack_id_tmp,rx_package_tmp.sender);
						display_backlight_off();
						//delivered_packages_array[delivered_packages++]=pack_id_tmp;
						FLASH_LED_OFF;
						return;
					}
				}
		FLASH_LED_OFF;

		/*
		FLASH_LED_ON;
		vTaskDelay(300);
		RF_modul_TxBuf((unsigned char*)&RF_RX_buffer_tmp[0], Bytes);
		FLASH_LED_OFF;
*/
		xQueueSend( xRfRxEventQueue, ( void * ) &rx_package_tmp, /*TIC_FQR*2*/ portMAX_DELAY );
		if (Bytes == sizeof(tPackage)){//если это пакет

		}
		return;


	}
	else{//пакет не получен, либо период закончился, либо в буфере изменения

		time_tmp =  xTaskGetTickCount();
		if  (unlock_time > time_tmp)//передача заблокирована
		{
			waiting_interval = unlock_time - time_tmp; //дождемся конца блокировки
			return;
		}
		if (xSemaphoreTake(rf_tx_buff_Semaphore, TIC_FQR*2/*portMAX_DELAY*/ )== pdTRUE)//ждем 2 секунды прерывание Si4432
		{


			if(rf_GetTxCount())//если буфер для отправки не пуст
			{
				if (rf_TxBuf[rf_txBufHead].nextTransmission > time_tmp){//рано отправлять пакет
				waiting_interval = rf_TxBuf[rf_txBufHead].nextTransmission - time_tmp;
				xSemaphoreGive(rf_tx_buff_Semaphore);
				return;
				}
				else //пора отправлять пакет
				{
					tWaitingPackage waiting_package_tmp;
					if (rf_GetWaitingPackage(&waiting_package_tmp))//если смогли забрать пакет из буфера
					{

						if (!waiting_package_tmp.package.details.needAck)//если пакет не требует подтверждения
						{
							RF_modul_TxBuf(&waiting_package_tmp.package, sizeof(tPackage));
//							translated_packages_array[translated_packages++]=waiting_package_tmp.package.details.packageId;
							unlock_time = xTaskGetTickCount() +BLOCKING_PERIOD_AFTE_TRANSMISSION;
							waiting_interval=BLOCKING_PERIOD_AFTE_TRANSMISSION;
							xSemaphoreGive(rf_tx_buff_Semaphore);
							return;

						}
						else//пакет требует подтверждения
						{
							//если подтверждение ещё не получено
							//и не закончился таймаут передачи

							//пропускаем все отправленные пакеты
							while ((waiting_package_tmp.isDeliveryComplite)&&(rf_GetTxCount()))
							{
								rf_GetWaitingPackage(&waiting_package_tmp);

							}
							if((!waiting_package_tmp.isDeliveryComplite)&&((waiting_package_tmp.wasCreated+waiting_package_tmp.timings.timeout) > time_tmp))
							{
								RF_modul_TxBuf(&waiting_package_tmp.package, sizeof(tPackage));//отправляем пакет
								waiting_package_tmp.nextTransmission = time_tmp + waiting_package_tmp.timings.resendTime + random(waiting_package_tmp.timings.resendTimeDelta);

								//translated_packages_array[translated_packages++]=waiting_package_tmp.package.details.packageId;

								if (rf_PutWaitingPackage(waiting_package_tmp)&&(rf_GetTxCount()>1))
								{
								//	rf_SortTxBuffByNextTransmission();

								}

							}

						}

					}
					unlock_time = time_tmp +BLOCKING_PERIOD_AFTE_TRANSMISSION;

					if(rf_GetTxCount()>0){
						if (rf_TxBuf[rf_txBufHead].nextTransmission > time_tmp)
						{
							waiting_interval = rf_TxBuf[rf_txBufHead].nextTransmission - time_tmp;
						}
						else
						{
							rf_TxBuf[rf_txBufHead].nextTransmission = time_tmp + waiting_package_tmp.timings.resendTime + random(waiting_package_tmp.timings.resendTimeDelta);
							waiting_interval =rf_TxBuf[rf_txBufHead].nextTransmission-time_tmp;//BLOCKING_PERIOD_AFTE_TRANSMISSION;
						}



					}
					else
					{
						waiting_interval = portMAX_DELAY;
					}

					xSemaphoreGive(rf_tx_buff_Semaphore);

					return;


				}



			}//[if(rf_GetTxCount())//если буфер для отправки не пуст]
			else{//если буфер для отправки пуст


					waiting_interval = portMAX_DELAY;
					xSemaphoreGive(rf_tx_buff_Semaphore);
					return;


			}//[else{//если буфер для отправки пуст]


			xSemaphoreGive(rf_tx_buff_Semaphore);
		}//[if (xSemaphoreTake(rf_tx_buff_Semaphore, 600/*portMAX_DELAY*/ )== pdTRUE)//ждем 2 секунды прерывание Si4432]

	}//	[else{//пакет не получен, либо период закончился, либо в буфере изменения]

}
/*
#define defaultTimeout 10000000
#define defaultResendTime 200000
#define  defaultResendTimeDelta 100000
*/

extern volatile tLastReceivedPackagesItem rf_lastRxBuff[RF_LAST_RECEIVED_PACKAGES_BUFFER_SIZE];
extern volatile unsigned char rf_lastRxBufTail;
extern volatile unsigned char rf_lastRxBufHead;
extern volatile unsigned char rf_lastRxCount;


void RF_modul_read_packages(void){
	static volatile tPackage package_tmp,test_package;

	const char data_to_send [] = "Hello!";
	static volatile portTickType waiting_interval = portMAX_DELAY;
	static volatile portTickType unlock_time = 0;
	static volatile portTickType time_tmp;
	static volatile tLastReceivedPackagesItem lastReceivedPackageItem_tmp;

#ifdef  ONLY_RECIEVER
	if (xQueueReceive( xRfRxEventQueue, (void*)&package_tmp, waiting_interval/*portMAX_DELAY*/)==pdPASS){//если прочитали
#else
		if (xQueueReceive( xRfRxEventQueue, (void*)&package_tmp, TIC_FQR*10)==pdPASS){//если прочитали
#endif

#ifdef ONLY_RECIEVER
//			received_packages_array[received_packages++]=package_tmp.details.packageId;


//		if(!((package_tmp.target.address[0]==my_rf_net_address.address[0])&&(package_tmp.target.address[1]==my_rf_net_address.address[1])&&(package_tmp.target.address[2]==my_rf_net_address.address[2])))return;//если адресованно не нам, выходим
/*
			if(received_packages==64)
		{
			int i;
			i++;
			received_packages=0;
		}
*/
//		vTaskDelay(300);
		time_tmp=xTaskGetTickCount();
		if (!rf_isLastPackageInBuf(package_tmp))//если пакета нет в буфере
		{
			received_unique_packages_array[received_unique_packages++]=package_tmp.details.packageId;
			lastReceivedPackageItem_tmp.id = package_tmp.details.packageId;
			lastReceivedPackageItem_tmp.sender=package_tmp.sender;
			lastReceivedPackageItem_tmp.removalTime = time_tmp+(defaultTimeout/1000000)*TIC_FQR;
			rf_PutLastRxPackage(lastReceivedPackageItem_tmp);
			waiting_interval=0;

			if(received_unique_packages==128){

				volatile int i;
				i++;
				received_unique_packages=0;
			}



			return;
		}
		else //пакет в буфере
		{
			waiting_interval=0;
			return;
		}


#endif


	}

	else {//не дождались пакета, таймаут
//		rf_send(target_rf_net_address, my_rf_net_address, data_to_send,6,true, timings_to_send);
//		rf_send(target_rf_net_address, my_rf_net_address, data_to_send,6,true, timings_to_send);
//		rf_send(target_rf_net_address, my_rf_net_address, data_to_send,6,true, timings_to_send);

#ifdef ONLY_RECIEVER
		time_tmp=xTaskGetTickCount();
		if (rf_GetLastRxCount()){
					if(time_tmp < rf_lastRxBuff[rf_lastRxBufHead].removalTime)//рано удалять пакет
					{

						waiting_interval = rf_lastRxBuff[rf_lastRxBufHead].removalTime- time_tmp;
						return;
					}
					else //пора удалять пакет
					{
						rf_GetLastRxPackage((tLastReceivedPackagesItem*)&lastReceivedPackageItem_tmp);
						if (rf_GetLastRxCount())//буфер не пустой
						{
							if(time_tmp < rf_lastRxBuff[rf_lastRxBufHead].removalTime)//рано удалять пакет
							{

								waiting_interval = rf_lastRxBuff[rf_lastRxBufHead].removalTime - time_tmp;
								return;
							}
							else//следующий пакет нужно удалить
							{
								waiting_interval = 0;
								return;
							}
						}
						else//буфер пустой
						{
							waiting_interval = portMAX_DELAY;
							return;
						}

					}

		}


		else//буфер пуст
		{
			waiting_interval =portMAX_DELAY;
			return;
		}

#endif



#ifndef ONLY_RECIEVER
		if (xSemaphoreTake(rf_tx_buff_Semaphore, TIC_FQR*2/*portMAX_DELAY*/ )== pdTRUE)//ждем 2 секунды прерывание Si4432
					{
					bool need_update=false;
					delivered_packages=0;
					translated_packages=0;
					for(int i=0;i<32;i++)
						{
							if (rf_GetTxCount() < RF_TX_BUFFER_SIZE)
							{
								rf_put_package_to_buffer(target_rf_net_address, my_rf_net_address, data_to_send,6,/*false*/true, timings_to_send);
								need_update = true;
							}
								//
							//rf_put_package_to_buffer(target_rf_net_address, my_rf_net_address, data_to_send,6,true, timings_to_send);
						}

						xSemaphoreGive(rf_tx_buff_Semaphore);
						if(need_update)	xSemaphoreGive( Si4432_IQR_Semaphore);
					}

#endif

	}

}


#endif //[#ifdef SI4432_ENABLE]
