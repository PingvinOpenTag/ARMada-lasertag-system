#ifndef __SI4432_H
#define __SI4432_H

#include "types.h"

#ifdef SI4432_ENABLE
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "rf_modul.h"





#define Si4432_Reg_Device_Type_Code 							0x00
#define Si4432_Reg_Verson_Code 									0x01
#define Si4432_Reg_Device_Status 								0x02
#define Si4432_Reg_Interrupt_Status_1 							0x03
#define Si4432_Reg_Interrupt_Status_2 							0x04
#define Si4432_Reg_Interrupt_Enable_1 							0x05
#define Si4432_Reg_Interrupt_Enable_2 							0x06
#define Si4432_Reg_Operating_Mode_1								0x07
#define Si4432_Reg_Operating_Mode_2 							0x08
#define Si4432_Reg_Crystal_Load_Capacitance 					0x09
#define Si4432_Reg_MCU_Output_Clock 							0x0A
#define Si4432_Reg_GPIO_Config_0 								0x0B
#define Si4432_Reg_GPIO_Config_1 								0x0C
#define Si4432_Reg_GPIO_Config_2 								0x0D
#define Si4432_Reg_IO_Port_Config 								0x0E
#define Si4432_Reg_ADC_Config 									0x0F
#define Si4432_Reg_ADC_Amplifier_Offset 						0x10
#define Si4432_Reg_ADC_Value 									0x11
#define Si4432_Reg_Temperature_Sensor_Calibration 				0x12
#define Si4432_Reg_Temperature_Value_Offset 					0x13
#define Si4432_Reg_Wakeup_Timer_Period_1 						0x14
#define Si4432_Reg_Wakeup_Timer_Period_2 						0x15
#define Si4432_Reg_Wakeup_Timer_Period_3 						0x16
#define Si4432_Reg_Wakeup_Timer_Value_1 						0x17
#define Si4432_Reg_Wakeup_Timer_Value_2 						0x18
#define Si4432_Reg_Low_Duty_Cycle_Mode_Duration 				0x19
#define Si4432_Reg_Low_Batter_Detector_Threshold 				0x1A
#define Si4432_Reg_Battery_Voltage_Level 						0x1B
#define Si4432_Reg_IF_FIlter_Bandwidth 							0x1C
#define Si4432_Reg_AFC_Loop_Gearshift_Override 					0x1D
#define Si4432_Reg_AFC_Timing_Ctrl 								0x1E
#define Si4432_Reg_Clock_Recovery_Gearshift_Override 			0x1F
#define Si4432_Reg_Clock_Recovery_Oversamping_Rate 				0x20
#define Si4432_Reg_Clock_Recovery_Offset_2 						0x21
#define Si4432_Reg_Clock_Recovery_Offset_1 						0x22
#define Si4432_Reg_Clock_Recovery_Offset_0 						0x23
#define Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_1 			0x24
#define Si4432_Reg_Clock_Recovery_Timing_Loop_Gain_0 			0x25
#define Si4432_Reg_Received_Signal_Strength_Indicator 			0x26
#define Si4432_Reg_RSSI_Threshold_For_Clear_Channel_Indicator 	0x27
#define Si4432_Reg_Antenna_Diversity_1 							0x28
#define Si4432_Reg_Antenna_Diversity_2 							0x29
#define Si4432_Reg_AFC_Limiter 									0x2A
#define Si4432_Reg_ADC_Correction 								0x2B
#define Si4432_Reg_OOK_Counter_Value_1 							0x2C
#define Si4432_Reg_OOK_Counter_Value_2 							0x2D
#define Si4432_Reg_Slicer_Peak_Holder 							0x2E
#define Si4432_Reg_Data_Access_Ctrl 							0x30
#define Si4432_Reg_EZMAC_Status 								0x31
#define Si4432_Reg_Header_Ctrl_1 								0x32
#define Si4432_Reg_Header_Ctrl_2 								0x33
#define Si4432_Reg_Preamble_Lenth 								0x34
#define Si4432_Reg_Preamble_Detection_Ctrl 						0x35
#define Si4432_Reg_Synchronization_Word_3 						0x36
#define Si4432_Reg_Synchronization_Word_2						0x37
#define Si4432_Reg_Synchronization_Word_1 						0x38
#define Si4432_Reg_Synchronization_Word_0 						0x39
#define Si4432_Reg_Transmit_Header_3 							0x3A
#define Si4432_Reg_Transmit_Header_2 							0x3B
#define Si4432_Reg_Transmit_Header_1 							0x3C
#define Si4432_Reg_Transmit_Header_0 							0x3D
#define Si4432_Reg_Packet_Length 								0x3E
#define Si4432_Reg_Check_Header_3 								0x3F
#define Si4432_Reg_Check_Header_2 								0x40
#define Si4432_Reg_Check_Header_1 								0x41
#define Si4432_Reg_Check_Header_0 								0x42
#define Si4432_Reg_Header_Enable_3 								0x43
#define Si4432_Reg_Header_Enable_2 								0x44
#define Si4432_Reg_Header_Enable_1 								0x45
#define Si4432_Reg_Header_Enable_0 								0x46
#define Si4432_Reg_Received_Header_3 							0x47
#define Si4432_Reg_Received_Header_2 							0x48
#define Si4432_Reg_Received_Header_1 							0x49
#define Si4432_Reg_Received_Header_0 							0x4A
#define Si4432_Reg_Receive_Packet_Length 						0x4B
#define Si4432_Reg_ADC8_Ctrl 									0x4F
#define Si4432_Reg_Channel_Filter_Coefficient_Offset 			0x60
#define Si4432_Reg_Crystal_And_Power_On_Reset_Ctrl 				0x62
#define Si4432_Reg_AGC_Override_1 								0x69
#define Si4432_Reg_TX_Power 									0x6D
#define Si4432_Reg_Tx_Data_Rate_1 								0x6E
#define Si4432_Reg_Tx_Data_Rate_0 								0x6F
#define Si4432_Reg_Modulation_Mode_Ctrl_1 						0x70
#define Si4432_Reg_Modulation_Mode_Ctrl_2 						0x71
#define Si4432_Reg_Frequency_Deviation 							0x72
#define Si4432_Reg_Frequency_Offset_1 							0x73
#define Si4432_Reg_Frequency_Offset_2 							0x74
#define Si4432_Reg_Frequency_Band_Select 						0x75
#define Si4432_Reg_Nominal_Carrier_Frequency_H 					0x76
#define Si4432_Reg_Nominal_Carrier_Frequency_L 					0x77
#define Si4432_Reg_Frequency_Hopping_Channel_Select 			0x79
#define Si4432_Reg_Frequency_Hopping_Step_Size 					0x7A
#define Si4432_Reg_Tx_FIFO_Ctrl_1 								0x7C
#define Si4432_Reg_Tx_FIFO_Ctrl_2 								0x7D
#define Si4432_Reg_Rx_FIFO_Ctrl 								0x7E
#define Si4432_Reg_FIFO_Access 									0x7F
/*!
 * @}
 */

/*!
 * \addtogroup Command
 * @{
 */
#define Si4432_IT1_FIFO_Over_And_Under_Flow 				0x80
#define Si4432_IT1_Tx_FIFO_Almost_Full 						0x40
#define Si4432_IT1_Tx_FIFO_Almost_Empty 					0x20
#define Si4432_IT1_Rx_FIFO_Almost_Full 						0x10
#define Si4432_IT1_External_Interrupt 						0x08
#define Si4432_IT1_Packet_Sent 								0x04
#define Si4432_IT1_Valid_Packed_Received 					0x02
#define Si4432_IT1_CRC_Error 								0x01
#define Si4432_IT1_Disable 									0x00
#define Si4432_IT2_Sync_Word_Detected 						0x80
#define Si4432_IT2_Valid_Preamble_Detected 					0x40
#define Si4432_IT2_Invalid_Preamble_Detected 				0x20
#define Si4432_IT2_RSSI 									0x10
#define Si4432_IT2_Wakeup_Timer 							0x08
#define Si4432_IT2_Low_Battery_Detected 					0x04
#define Si4432_IT2_Chip_Ready 								0x02
#define Si4432_IT2_POR 										0x01
#define Si4432_IT2_Disable 									0x00
#define Si4432_Mode1_Soft_Reset								0x80
#define Si4432_Mode1_Low_Battery_Detect 					0x40
#define Si4432_Mode1_Wakeup_Timer 							0x20
#define Si4432_Mode1_Crystal_32K 							0x00
#define Si4432_Mode1_Crystal_RC 							0x10
#define Si4432_Mode1_Tx_On 									0x08
#define Si4432_Mode1_Rx_On 									0x04
#define Si4432_Mode1_Tune 									0x02
#define Si4432_Mode1_Ready 									0x01
#define Si4432_Mode2_Rx_Multi_Packet 						0x10
#define Si4432_Mode2_Auto_Transmission 						0x08
#define Si4432_Mode2_Low_Duty_Cycle 						0x04
#define Si4432_Mode2_Rx_FIFO_Reset 							0x02
#define Si4432_Mode2_Tx_FIFO_Reset 							0x01
#define Si4432_MCU_0_Cycles 								0x00
#define Si4432_MCU_128_Cycles 								0x10
#define Si4432_MCU_256_Cycles 								0x20
#define Si4432_MCU_512_Cycles 								0x30
#define Si4432_MCU_Enable_Low_Frequency_Clock 				0x80
#define Si4432_MCU_Clock_30Mhz 								0x00
#define Si4432_MCU_Clock_15Mhz 								0x01
#define Si4432_MCU_Clock_10Mhz 								0x02
#define Si4432_MCU_Clock_4Mhz 								0x03
#define Si4432_MCU_Clock_3Mhz 								0x04
#define Si4432_MCU_Clock_2Mhz 								0x05
#define Si4432_MCU_Clock_1Mhz 								0x06
#define Si4432_MCU_Clock_32khz 								0x07
#define Si4432_MCU_Clear 									0x00
#define Si4432_GPIO_Pullup 									0x20
#define Si4432_GPIO_Power_On_Reset 							0x00
#define Si4432_GPIO_Wakeup_Timer 							0x01
#define Si4432_GPIO_Low_Battery_Detect 						0x02
#define Si4432_GPIO_Digital_Input		 					0x03
#define Si4432_GPIO_EXIT_Falling 							0x04
#define Si4432_GPIO_EXIR_Rising 							0x05
#define Si4432_GPIO_EXIT_Chang 								0x06
#define Si4432_GPIO_Analog_Input 							0x07
#define Si4432_GPIO_R_Analog_Test_N_In 						0x08
#define Si4432_GPIO_R_Analog_Test_P_In 						0x09
#define Si4432_GPIO_Digital_Output 							0x0A
#define Si4432_GPIO_R_Digital_Test 							0x0B
#define Si4432_GPIO_R_Analog_Test_N_Out 					0x0C
#define Si4432_GPIO_R_Analog_Test_P_Out 					0x0D
#define Si4432_GPIO_Reference_Voltage 						0x0E
#define Si4432_GPIO_Tx_Rx_Clock_Output 						0x0F
#define Si4432_GPIO_Tx_Data_Input 							0x10
#define Si4432_GPIO_External_Retransmission 				0x11
#define Si4432_GPIO_Tx_State 								0x12
#define Si4432_GPIO_Tx_FIFO_Almost_Full 					0x13
#define Si4432_GPIO_Rx_Data_Out 							0x14
#define Si4432_GPIO_Rx_State 								0x15
#define Si4432_GPIO_Rx_FIFO_Almost_Full 					0x16
#define Si4432_GPIO_Antenna1 								0x17
#define Si4432_GPIO_Antenna2 								0x18
#define Si4432_GPIO_Valid_Preamble_Detected 				0x19
#define Si4432_GPIO_Invalid_Preamble_Detected 				0x1A
#define Si4432_GPIO_Sync_Word_Detected 						0x1B
#define Si4432_GPIO_Clear_Channel_Assessment 				0x1C
#define Si4432_GPIO_VDD 									0x1D
#define Si4432_GPIO_GND 									0x1E
#define Si4432_GPIO_Clear 									0x00






/* Default SPI used */
#ifndef SI4432_SPI_NUMBER
#define SI4432_SPI_NUMBER 2
#endif

#ifndef SI4432_SPI
#if SI4432_SPI_NUMBER==2
#define SI4432_SPI SPI2
#elif SI4432_SPI_NUMBER==3
#define SI4432_SPI SPI3
#endif
#endif

/* SPI chip enable pin (PA6 for SPI3, ? for SPI3)*/
#ifndef SI4432_nSEL_PIN
#if SI4432_SPI_NUMBER==2
#define SI4432_nSEL_PORT			 			GPIOA
#define SI4432_nSEL_PIN			 				GPIO_Pin_6
#define SI4432_nSEL_RCC_APB_PeriphClockCmd	 	RCC_APB2PeriphClockCmd
#define SI4432_nSEL_RCC_APB_Periph_GPIO	 		RCC_APB2Periph_GPIOA
#elif SI4432_SPI_NUMBER==3
#define SI4432_nSEL_PORT			 			GPIOB
#define SI4432_nSEL_PIN			 				GPIO_Pin_9
#define SI4432_nSEL_RCC_APB_PeriphClockCmd	 	RCC_APB2PeriphClockCmd
#define SI4432_nSEL_RCC_APB_Periph_GPIO	 		RCC_APB2Periph_GPIOB
#endif
#endif

/* Chip enable (PC3)*/
#ifndef SI4432_SDN_PIN
#if SI4432_SPI_NUMBER==2
#define SI4432_SDN_PORT			 				GPIOC
#define SI4432_SDN_PIN				 			GPIO_Pin_3
#define SI4432_SDN_RCC_APB_PeriphClockCmd	 	RCC_APB2PeriphClockCmd
#define SI4432_SDN_RCC_APB_Periph_GPIO		 	RCC_APB2Periph_GPIOC
#elif SI4432_SPI_NUMBER==3
#define SI4432_SDN_PORT			 				GPIOB
#define SI4432_SDN_PIN				 			GPIO_Pin_7
#define SI4432_SDN_RCC_APB_PeriphClockCmd	 	RCC_APB2PeriphClockCmd
#define SI4432_SDN_RCC_APB_Periph_GPIO		 	RCC_APB2Periph_GPIOB
#endif
#endif


/* Pins configuration */
#define SI4432_nSEL_LOW			GPIO_ResetBits(SI4432_nSEL_PORT, SI4432_nSEL_PIN)
#define SI4432_nSEL_HIGH		GPIO_SetBits(SI4432_nSEL_PORT, SI4432_nSEL_PIN)
#define SI4432_SDN_LOW			GPIO_ResetBits(SI4432_SDN_PORT, SI4432_SDN_PIN)
#define SI4432_SDN_HIGH			GPIO_SetBits(SI4432_SDN_PORT, SI4432_SDN_PIN)

/*IQR pin */
#ifndef SI4432_IQR_PIN
#if SI4432_SPI_NUMBER==2
#define SI4432_IQR_PORT			 				GPIOA
#define SI4432_IQR_PIN			 				GPIO_Pin_0
#define SI4432_IQR_RCC_APB_PeriphClockCmd	 	RCC_APB2PeriphClockCmd
#define SI4432_IQR_RCC_APB_Periph_GPIO	 		RCC_APB2Periph_GPIOA
#define SI4432_IQR_GPIO_PortSource 				GPIO_PortSourceGPIOA
#define SI4432_IQR_GPIO_PinSource 				GPIO_PinSource0
#define SI4432_IQR_EXTI_Line 					EXTI_Line0
#define SI4432_IQR_EXTI_IRQn 					EXTI0_IRQn
#define SI4432_IRQHandler						EXTI0_IRQHandler
#elif SI4432_SPI_NUMBER==3
#define SI4432_IQR_PORT			 				GPIOB
#define SI4432_IQR_PIN			 				GPIO_Pin_8
#define SI4432_IQR_RCC_APB_PeriphClockCmd	 	RCC_APB2PeriphClockCmd
#define SI4432_IQR_RCC_APB_Periph_GPIO	 		RCC_APB2Periph_GPIOB
#define SI4432_IQR_GPIO_PortSource 				GPIO_PortSourceGPIOB
#define SI4432_IQR_GPIO_PinSource 				GPIO_PinSource8
#define SI4432_IQR_EXTI_Line 					EXTI_Line8
#define SI4432_IQR_EXTI_IRQn 					EXTI9_5_IRQn
#define SI4432_IRQHandler						EXTI9_5_IRQHandler
#endif
#endif


#define Bit(bit)  (1<<(bit))




#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
//пример: ClearBit(PORTB, 1); //сбросить 1-й бит PORTB

#define SetBit(reg, bit)          reg |= (1<<(bit))

#define BitIsClear(reg, bit)    ((reg & (1<<(bit))) == 0)
//пример: if (BitIsClear(PORTB,1)) {...} //если бит очищен

#define BitIsSet(reg, bit)       ((reg & (1<<(bit))) != 0)
//пример: if(BitIsSet(PORTB,2)) {...} //если бит установлен

#define InvBit(reg, bit)          reg ^= (1<<(bit))
//пример: InvBit(PORTB, 1); //инвертировать 1-й бит PORTB


//extern void si4432_IQR_callback(void);

void init_spi_for_si4432(void);
void init_gpio_for_si4432(void);
void init_isr_for_si4432(void);
u8 si4432_spi_readWrite(u8 byte);
unsigned char si4432_spi_readReg(unsigned char reg);
void si4432_spi_writeReg(unsigned char reg, unsigned char value);
void si4432_spi_readBuf(unsigned char reg,unsigned char *pBuf, unsigned char bytes);
void si4432_spi_writeBuf(unsigned char reg, unsigned char *pBuf, unsigned char bytes);

#endif //[#ifdef SI4432_ENABLE]
#endif //[#ifndef __SI4432_H]
