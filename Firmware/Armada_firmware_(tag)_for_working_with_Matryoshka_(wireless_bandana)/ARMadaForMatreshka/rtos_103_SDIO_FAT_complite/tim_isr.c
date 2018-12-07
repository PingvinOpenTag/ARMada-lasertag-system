#include "stm32f10x.h"
#include "miles_protocol.h"
#include "global_variables.h"
#include "types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "si4432.h"

extern TKEYBOARD_EVENT test_reload_key(int adc_value);//Проверяем события клавиши "Перезарядить"
extern TKEYBOARD_EVENT test_keyboard(int adc_value);//Проверяем события клавиатуры
extern tfire_mode test_fire_mode_switch(void);
extern void lcd8544_dma_refresh(void);
extern void startSPI2(void);
extern void set_zone_buffer_bit (TDamageZone, uint8_t, bool);

void TIM2_IRQHandler(void)//2-я зона
{

	static portBASE_TYPE xHigherPriorityTaskWoken;
	  xHigherPriorityTaskWoken = pdFALSE;
	uint16_t cnt1, cnt2;
	        if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) {
	                TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	                TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	                cnt1 = TIM_GetCapture1(TIM2);
	                cnt2 = TIM_GetCapture2(TIM2);

	                if (tim2_rec_state == REC_Recording) {

	                	if (tim2_ir_rx_count  < IR_RAW_BUFFER_SIZE) {
	                		if(tim2_ir_rx_count==0)//первым должен идти заголовок
	                		{
	                			if(checkVal(cnt2,IR_START_BIT_DURATION,IR_TOL))//проверяем заголовок на ошибку
	                			{
	                				tim2_ir_rx_count++;
	                			}
	                			else //заголовок "битый"
	                			{
	                				// запрещаем прерывания по переполнению третьего канала
	                				TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim2_rec_state = REC_Idle;
	                				tim2_ir_rx_count=0;
								}
	                		}
	                		else //заголовок уже получен, идет прием данных
	                		{
//	                			TIM2_IR_RX_RAW_BUFFER[tim2_ir_rx_count-1].Period=cnt1;
//	                	        TIM2_IR_RX_RAW_BUFFER[tim2_ir_rx_count-1].Pulse=cnt2;

	                			if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ONE_BIT_DURATION+IR_SPACE_DURATION, IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_2,tim2_ir_rx_count-1,true);//это единица
	                				tim2_ir_rx_count++;
	                			}
	                			else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ZERO_BIT_DURATION+IR_ZERO_BIT_DURATION,IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_2,tim2_ir_rx_count-1,false);//это ноль
	                				tim2_ir_rx_count++;
	                			} else {//ошибка
	                				TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim2_rec_state = REC_Idle;
	                				tim2_ir_rx_count=0;
								}



	                		}
	                        }
	                	else {//слишком длинный пакет - ошибка
	                		 // запрещаем прерывания по переполнению третьего канала
	                		  	TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                		   	tim2_rec_state = REC_Idle;
	                		   	tim2_ir_rx_count=0;
						}

	                }


	                if (tim2_rec_state == REC_Idle) {
	                        //Пришел первый фронт, начинаем запись
	                	//разрешаем прерывания по 3 каналу, предварительно очистив флаг
	                	TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	                	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
	                	tim2_rec_state = REC_Recording;
	//                        captCount = 0;

	                }

	        }

	        if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET) {//если таймаут по приёму
	                TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	                if (tim2_rec_state == REC_Recording) {

	                	if((tim2_ir_rx_count==14)||(tim2_ir_rx_count==24))
	                	{
	                		cnt2 = TIM_GetCapture2(TIM2);
	                		if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL))
	                		{
	                			set_zone_buffer_bit(zone_2,tim2_ir_rx_count-1,true);//это единица
//	                			tim2_ir_rx_count++;
	                		}
	                		else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL))
	                		{
	                		set_zone_buffer_bit(zone_2,tim2_ir_rx_count-1,false);//это ноль
	            //    		tim2_ir_rx_count++;
	                		} else {//ошибка
	                	   				TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                	   				tim2_rec_state = REC_Idle;
	                            		tim2_ir_rx_count=0;
	        	                		Zone2RxEvent=RX_ERROR;
	        	                		xSemaphoreGiveFromISR( xZone2Semaphore, &xHigherPriorityTaskWoken);

	                				}

	                		tim2_rec_state = REC_Captured;
	                		if(tim2_ir_rx_count==14)Zone2RxEvent=RX_COMPLETE;
	                		else Zone2RxEvent=RX_MESSAGE_COMPLETE;
//	                		xTaskGetTickCount();
		                	xSemaphoreGiveFromISR( xZone2Semaphore, &xHigherPriorityTaskWoken );

	                	}
	                	else {//ошибка
	                		tim2_rec_state = REC_Idle;
	                		Zone2RxEvent=RX_ERROR;
	                		xSemaphoreGiveFromISR( xZone2Semaphore, &xHigherPriorityTaskWoken );
						}

	                	// запрещаем прерывания по переполнению третьего канала
	                	TIM_ITConfig(TIM2, TIM_IT_CC3, /*ENABLE*/DISABLE);
	   //             	tim5_rec_state = REC_Idle;

	                	tim2_ir_rx_count=0;
	                	//xSemaphoreGiveFromISR( xZone4Semaphore, &xHigherPriorityTaskWoken );
	                }

	        }


}





void TIM3_IRQHandler(void)//3-я зона
{

	static portBASE_TYPE xHigherPriorityTaskWoken;
	  xHigherPriorityTaskWoken = pdFALSE;
	uint16_t cnt1, cnt2;
	        if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
	                TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	                TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

	                cnt1 = TIM_GetCapture1(TIM3);
	                cnt2 = TIM_GetCapture2(TIM3);

	                if (tim3_rec_state == REC_Recording)
	                {

	                	if (tim3_ir_rx_count  < IR_RAW_BUFFER_SIZE) {
	                		if(tim3_ir_rx_count==0)//первым должен идти заголовок
	                		{
	                			if(checkVal(cnt2,IR_START_BIT_DURATION,IR_TOL))//проверяем заголовок на ошибку
	                			{
	                				tim3_ir_rx_count++;
	                			}
	                			else //заголовок "битый"
	                			{
	                				// запрещаем прерывания по переполнению третьего канала
	                				TIM_ITConfig(TIM3, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim3_rec_state = REC_Idle;
	                				tim3_ir_rx_count=0;
								}
	                		}
	                		else //заголовок уже получен, идет прием данных
	                		{
//	                			TIM3_IR_RX_RAW_BUFFER[tim3_ir_rx_count-1].Period=cnt1;
//	                	        TIM3_IR_RX_RAW_BUFFER[tim3_ir_rx_count-1].Pulse=cnt2;

	                			if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ONE_BIT_DURATION+IR_SPACE_DURATION, IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_3,tim3_ir_rx_count-1,true);//это единица
	                				tim3_ir_rx_count++;
	                			}
	                			else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ZERO_BIT_DURATION+IR_ZERO_BIT_DURATION,IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_3,tim3_ir_rx_count-1,false);//это ноль
	                				tim3_ir_rx_count++;
	                			} else {//ошибка
	                				TIM_ITConfig(TIM3, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim3_rec_state = REC_Idle;
	                				tim3_ir_rx_count=0;
								}



	                		}
	                        }
	                	else {//слишком длинный пакет - ошибка
	                		 // запрещаем прерывания по переполнению третьего канала
	                		  	TIM_ITConfig(TIM3, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                		   	tim3_rec_state = REC_Idle;
	                		   	tim3_ir_rx_count=0;
						}

	                }


	                if (tim3_rec_state == REC_Idle) {
	                        //Пришел первый фронт, начинаем запись
	                	//разрешаем прерывания по 3 каналу, предварительно очистив флаг
	                	TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
	                	TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
	                	tim3_rec_state = REC_Recording;
	//                        captCount = 0;

	                }

	        }//[if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) ]

	        if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET) {//если таймаут по приёму
	                TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
	                if (tim3_rec_state == REC_Recording) {

	                	if((tim3_ir_rx_count==14)||(tim3_ir_rx_count==24))
	                	{
	                		cnt2 = TIM_GetCapture2(TIM3);
	                		if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL))
	                		{
	                			set_zone_buffer_bit(zone_3,tim3_ir_rx_count-1,true);//это единица
//	                			tim3_ir_rx_count++;
	                		}
	                		else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL))
	                		{
	                		set_zone_buffer_bit(zone_3,tim3_ir_rx_count-1,false);//это ноль
//	                		tim3_ir_rx_count++;
	                		} else {//ошибка
	                	   				TIM_ITConfig(TIM3, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                	   				tim3_rec_state = REC_Idle;
	                            		tim3_ir_rx_count=0;
	        	                		Zone3RxEvent=RX_ERROR;
	        	                		xSemaphoreGiveFromISR( xZone3Semaphore, &xHigherPriorityTaskWoken);

	                				}

	                		tim3_rec_state = REC_Captured;
	                		if(tim3_ir_rx_count==14)Zone3RxEvent=RX_COMPLETE;
	                		else Zone3RxEvent=RX_MESSAGE_COMPLETE;
		                	xSemaphoreGiveFromISR( xZone3Semaphore, &xHigherPriorityTaskWoken );

	                	}
	                	else {//ошибка
	                		tim3_rec_state = REC_Idle;
	                		Zone3RxEvent=RX_ERROR;
	                		xSemaphoreGiveFromISR( xZone3Semaphore, &xHigherPriorityTaskWoken );
						}

	                	// запрещаем прерывания по переполнению третьего канала
	                	TIM_ITConfig(TIM3, TIM_IT_CC3, /*ENABLE*/DISABLE);
	   //             	tim5_rec_state = REC_Idle;

	                	tim3_ir_rx_count=0;
	                	//xSemaphoreGiveFromISR( xZone4Semaphore, &xHigherPriorityTaskWoken );
	                }

	        }


}






void TIM4_IRQHandler(void)//1-я зон
{

	static portBASE_TYPE xHigherPriorityTaskWoken;
	  xHigherPriorityTaskWoken = pdFALSE;
	uint16_t cnt1, cnt2;
	        if (TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET) {
	                TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
	                TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	                cnt1 = TIM_GetCapture1(TIM4);
	                cnt2 = TIM_GetCapture2(TIM4);

	                if (tim4_rec_state == REC_Recording) {

	                	if (tim4_ir_rx_count  < IR_RAW_BUFFER_SIZE) {
	                		if(tim4_ir_rx_count==0)//первым должен идти заголовок
	                		{
	                			if(checkVal(cnt2,IR_START_BIT_DURATION,IR_TOL))//проверяем заголовок на ошибку
	                			{
	                				tim4_ir_rx_count++;
	                			}
	                			else //заголовок "битый"
	                			{
	                				// запрещаем прерывания по переполнению третьего канала
	                				TIM_ITConfig(TIM4, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim4_rec_state = REC_Idle;
	                				tim4_ir_rx_count=0;
								}
	                		}
	                		else //заголовок уже получен, идет прием данных
	                		{
//	                			TIM4_IR_RX_RAW_BUFFER[tim4_ir_rx_count-1].Period=cnt1;
//	                	        TIM4_IR_RX_RAW_BUFFER[tim4_ir_rx_count-1].Pulse=cnt2;

	                			if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ONE_BIT_DURATION+IR_SPACE_DURATION, IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_1,tim4_ir_rx_count-1,true);//это единица
	                				tim4_ir_rx_count++;
	                			}
	                			else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ZERO_BIT_DURATION+IR_ZERO_BIT_DURATION,IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_1,tim4_ir_rx_count-1,false);//это ноль
	                				tim4_ir_rx_count++;
	                			} else {//ошибка
	                				TIM_ITConfig(TIM4, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim4_rec_state = REC_Idle;
	                				tim4_ir_rx_count=0;
								}



	                		}
	                        }
	                	else {//слишком длинный пакет - ошибка
	                		 // запрещаем прерывания по переполнению третьего канала
	                		  	TIM_ITConfig(TIM4, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                		   	tim4_rec_state = REC_Idle;
	                		   	tim4_ir_rx_count=0;
						}

	                }


	                if (tim4_rec_state == REC_Idle) {
	                        //Пришел первый фронт, начинаем запись
	                	//разрешаем прерывания по 3 каналу, предварительно очистив флаг
	                	TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
	                	TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
	                	tim4_rec_state = REC_Recording;
	//                        captCount = 0;

	                }

	        }

	        if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET) {//если таймаут по приёму
	                TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);
	                if (tim4_rec_state == REC_Recording) {

	                	if((tim4_ir_rx_count==14)||(tim4_ir_rx_count==24))
	                	{
	                		cnt2 = TIM_GetCapture2(TIM4);
	                		if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL))
	                		{
	                			set_zone_buffer_bit(zone_1,tim4_ir_rx_count-1,true);//это единица
//	                			tim4_ir_rx_count++;
	                		}
	                		else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL))
	                		{
	                		set_zone_buffer_bit(zone_1,tim4_ir_rx_count-1,false);//это ноль
//	                		tim4_ir_rx_count++;
	                		} else {//ошибка
	                	   				TIM_ITConfig(TIM4, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                	   				tim4_rec_state = REC_Idle;
	                            		tim4_ir_rx_count=0;
	        	                		Zone1RxEvent=RX_ERROR;
	        	                		xSemaphoreGiveFromISR( xZone1Semaphore, &xHigherPriorityTaskWoken);

	                				}

	                		tim4_rec_state = REC_Captured;
	                		if (tim4_ir_rx_count==14) Zone1RxEvent=RX_COMPLETE;
	                		else Zone1RxEvent = RX_MESSAGE_COMPLETE;
		                	xSemaphoreGiveFromISR( xZone1Semaphore, &xHigherPriorityTaskWoken );

	                	}
	                	else {//ошибка
	                		tim4_rec_state = REC_Idle;
	                		Zone1RxEvent=RX_ERROR;
	                		xSemaphoreGiveFromISR( xZone1Semaphore, &xHigherPriorityTaskWoken );
						}

	                	// запрещаем прерывания по переполнению третьего канала
	                	TIM_ITConfig(TIM4, TIM_IT_CC3, /*ENABLE*/DISABLE);
	   //             	tim5_rec_state = REC_Idle;

	                	tim4_ir_rx_count=0;
	                	//xSemaphoreGiveFromISR( xZone4Semaphore, &xHigherPriorityTaskWoken );
	                }

	        }


}


void TIM5_IRQHandler(void)//4-я зона
{

	static portBASE_TYPE xHigherPriorityTaskWoken;
	  xHigherPriorityTaskWoken = pdFALSE;
	uint16_t cnt1, cnt2;
	        if (TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) {
	                TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
	                TIM_ClearITPendingBit(TIM5, TIM_IT_Update);

	                cnt1 = TIM_GetCapture1(TIM5);
	                cnt2 = TIM_GetCapture2(TIM5);

	                if (tim5_rec_state == REC_Recording) {

	                	if (tim5_ir_rx_count  < IR_RAW_BUFFER_SIZE) {
	                		if(tim5_ir_rx_count==0)//первым должен идти заголовок
	                		{
	                			if(checkVal(cnt2,IR_START_BIT_DURATION,IR_TOL))//проверяем заголовок на ошибку
	                			{

	                		//		captArray[captCount*2] = cnt1; //width
	                        //        captArray[captCount*2+1] = cnt2; //pulse
	                        //        captCount++;
	                				tim5_ir_rx_count++;
	                			}
	                			else //заголовок "битый"
	                			{
	                				// запрещаем прерывания по переполнению третьего канала
	                				TIM_ITConfig(TIM5, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                				tim5_rec_state = REC_Idle;
	                			tim5_ir_rx_count=0;
								}
	                		}
	                		else //заголовок уже получен, идет прием данных
	                		{
//	                			TIM5_IR_RX_RAW_BUFFER[tim5_ir_rx_count-1].Period=cnt1;
//	                	        TIM5_IR_RX_RAW_BUFFER[tim5_ir_rx_count-1].Pulse=cnt2;

	                			if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ONE_BIT_DURATION+IR_SPACE_DURATION, IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_4,tim5_ir_rx_count-1,true);//это единица
	                				tim5_ir_rx_count++;
	                			}
	                			else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL)&&checkVal(cnt1,IR_ZERO_BIT_DURATION+IR_ZERO_BIT_DURATION,IR_TOL))
	                			{
	                				set_zone_buffer_bit(zone_4,tim5_ir_rx_count-1,false);//это ноль
	                				tim5_ir_rx_count++;
	                			} else {//ошибка
	                				TIM_ITConfig(TIM5, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                					                				tim5_rec_state = REC_Idle;
	                					                			tim5_ir_rx_count=0;
								}



	                		}
	                        }
	                	else {//слишком длинный пакет - ошибка
	                		 // запрещаем прерывания по переполнению третьего канала
	                		  	TIM_ITConfig(TIM5, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                		   	tim5_rec_state = REC_Idle;
	                		   	tim5_ir_rx_count=0;
						}

	                }


	                if (tim5_rec_state == REC_Idle) {
	                        //Пришел первый фронт, начинаем запись
	                	//разрешаем прерывания по 3 каналу, предварительно очистив флаг
	                	TIM_ClearITPendingBit(TIM5, TIM_IT_CC3);
	                	TIM_ITConfig(TIM5, TIM_IT_CC3, ENABLE);
	                	tim5_rec_state = REC_Recording;
	//                        captCount = 0;

	                }

	        }

	        if (TIM_GetITStatus(TIM5, TIM_IT_CC3) != RESET) {//если таймаут по приёму
	                TIM_ClearITPendingBit(TIM5, TIM_IT_CC3);
	                if (tim5_rec_state == REC_Recording) {

	                	if((tim5_ir_rx_count==14)||(tim5_ir_rx_count==24))//получили заголовок и 13 бит данных
	                	{
	                		cnt2 = TIM_GetCapture2(TIM5);
	                		if(checkVal(cnt2,IR_ONE_BIT_DURATION,IR_TOL))
	                		{
	                			set_zone_buffer_bit(zone_4,tim5_ir_rx_count-1,true);//это единица
//	                			tim5_ir_rx_count++;
	                		}
	                		else if (checkVal(cnt2, IR_ZERO_BIT_DURATION,IR_TOL))
	                		{
	                		set_zone_buffer_bit(zone_4,tim5_ir_rx_count-1,false);//это ноль
//	                		tim5_ir_rx_count++;
	                		} else {//ошибка
	                	   				TIM_ITConfig(TIM5, TIM_IT_CC3, /*ENABLE*/DISABLE);
	                	   				tim5_rec_state = REC_Idle;
	                            		tim5_ir_rx_count=0;
	        	                		Zone4RxEvent=RX_ERROR;
	        	                		xSemaphoreGiveFromISR( xZone4Semaphore, &xHigherPriorityTaskWoken);

	                				}

	                		tim5_rec_state = REC_Captured;
	                		if(tim5_ir_rx_count==14) Zone4RxEvent=RX_COMPLETE;
	                		else Zone4RxEvent=RX_MESSAGE_COMPLETE;
	                		xSemaphoreGiveFromISR( xZone4Semaphore, &xHigherPriorityTaskWoken );

	                	}
	                	else {//ошибка
	                		tim5_rec_state = REC_Idle;
	                		tim5_ir_rx_count=0;
	                		Zone4RxEvent=RX_ERROR;
	                		xSemaphoreGiveFromISR( xZone4Semaphore, &xHigherPriorityTaskWoken );
						}

	                	// запрещаем прерывания по переполнению третьего канала
	                	TIM_ITConfig(TIM5, TIM_IT_CC3, /*ENABLE*/DISABLE);
	   //             	tim5_rec_state = REC_Idle;

	                	tim5_ir_rx_count=0;
	                	//xSemaphoreGiveFromISR( xZone4Semaphore, &xHigherPriorityTaskWoken );
	                }

	        }

}





void TIM7_IRQHandler(void)
{
	//static signed BaseType_t xHigherPriorityTaskWoken;

	static portBASE_TYPE xHigherPriorityTaskWoken;
	  xHigherPriorityTaskWoken = pdFALSE;
	static /*volatile*/ tsystem_event_type sys_event_tmp;
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update);

//if(delay_timer>0) delay_timer--;

#ifdef SI4432_ENABLE
#if SI4432_SPI_NUMBER==3
	startSPI2();//запускаем DMA для передачи по SPI

#endif
#else //[#ifdef SI4432_ENABLE]
	startSPI2();//запускаем DMA для передачи по SPI
#endif

#ifdef SENSORS_BECKLIGHT
static volatile div=0;
	if (div++ >=NUMBER_OF_SENSORS_FRAMES)
	{
	div=0;
#endif
	if(safe_counter>0) safe_counter--;
	if(countdown_shock_time>0)countdown_shock_time--;



	static volatile int adc_val;

	adc_val = ADC_values[0];//ADC_GetConversionValue(ADC2);
static volatile int tim7_tic_cntr=0;

	if (tim7_tic_cntr >= 5)
	{
#ifdef DISPLAY_ENABLE
#ifndef COLOR_LCD
		if(screen_auto_refresh)lcd8544_dma_refresh();
#endif
#endif
		tim7_tic_cntr=0;
		curr_fire_mode = test_fire_mode_switch();
		if (last_fire_mode != curr_fire_mode)
		{
static /*volatile*/ tsystem_event_type fire_switch_event;
			last_fire_mode = curr_fire_mode;
			fire_switch_event.event_source = FIRE_MODE_SWITCH;
			fire_switch_event.event_code = (char)curr_fire_mode;
			xQueueSendToBackFromISR(xEventQueue, &fire_switch_event, &xHigherPriorityTaskWoken);
			if( xHigherPriorityTaskWoken == pdTRUE )
			                          {
			                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
			                          }
/*
#ifndef 	COLOR_LCD
			lcd_fire_mode_update(curr_fire_mode);
#else
			color_lcd_fire_mode_update(curr_fire_mode);
#endif
*/
		}



	}
	else tim7_tic_cntr++;



	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6))//проверяем наличие датчика №1
	{

	}
	if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15))//проверяем наличие датчика №2
	{

	}
	if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))//проверяем наличие датчика №3
	{

	}
	if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))//проверяем наличие датчика №4
	{

	}


	switch(keyboard_event)
				{
			  	case no_key_pressing:
			  		{
						keyboard_event=test_keyboard(adc_val);
						if( (keyboard_event!= no_key_pressing)||(reload_key_event!=no_key_pressing))  xSemaphoreGiveFromISR( xKeysSemaphore, &xHigherPriorityTaskWoken );
						if( xHigherPriorityTaskWoken == pdTRUE )
						                          {
						                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
						                          }

						if(keyboard_event!= no_key_pressing){
								sys_event_tmp.event_source = TRIGGER_KEY;
						//		sys_event_tmp.event_code = (char)keyboard_event;
								xQueueSendToBackFromISR(xEventQueue, &sys_event_tmp, &xHigherPriorityTaskWoken);
								if( xHigherPriorityTaskWoken == pdTRUE )
								                          {
								                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
								                          }
							}
						break;
					}
			  	default:;
				}
	switch( reload_key_event)
					{
				  	case no_key_pressing:
				  		{
				  			reload_key_event=test_reload_key(adc_val);

				  			if (reload_key_event!=no_key_pressing){
				  				sys_event_tmp.event_source = RELOAD_KEY;
				  		//		sys_event_tmp.event_code = (char)reload_key_event;
				  				xQueueSendToBackFromISR(xEventQueue, &sys_event_tmp, &xHigherPriorityTaskWoken);
				  				if( xHigherPriorityTaskWoken == pdTRUE )
				  				                          {
				  				                                portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
				  				                          }
				  			}

				  			break;
						}
				  	default:;
					}

#ifdef DIRECTION_CALCULATION
	if(hit_timeout_counter < HIT_TIMEOUT){
		hit_timeout_counter++;
	}
	else if (hit_timeout_counter==HIT_TIMEOUT){
		hit_timeout_counter = 0xFF;
		xSemaphoreGiveFromISR(xHitDirectionSemaphore, &xHigherPriorityTaskWoken);
	}
#endif


#ifdef SENSORS_BECKLIGHT
	}
#endif
}



void TIM8_CC_IRQHandler(void){
//	ITStatus stat_tmp;
//		stat_tmp = TIM_GetITStatus(TIM8, TIM_IT_CC1);
		if (TIM_GetITStatus(TIM8, TIM_IT_CC1) != RESET)
	  {
			/* Даём знать, что обработали прерывание */
	    TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);
	   	TIM1->CCR1 = 0;
	  }
}



void TIM8_UP_IRQHandler(void)
{

    /* Тут как-нибудь обрабатываем событие over-capture, если провороним */

	if (TIM_GetFlagStatus(TIM8, TIM_SR_UIF /*TIM_DIER_UIE*//*TIM_FLAG_CC1OF*/) != RESET)
    {
		TIM_ClearFlag(TIM8, TIM_SR_UIF/*TIM_DIER_UIE*//*TIM_FLAG_CC1OF*/);
      //f_status = TIM_GetFlagStatus(TIM8, TIM_SR_UIF /*TIM_DIER_UIE*//*TIM_FLAG_CC1OF*/);
    if (ir_tx_buffer_cursor.bits_for_tx>0) //если указатель указывает не на пустую ячейку
    		{
    			TIM1->CCR1 = 50; // включаем несущую ИК
    			if(!ir_tx_buffer_cursor.heder_been_sent)
    			{
    				ir_tx_buffer_cursor.heder_been_sent = true;
    				return;
    			}
    			if(ir_tx_buffer_cursor.bit_mask == 0)//все биты текущего байта уже переданы
    			{
    				ir_tx_buffer_cursor.byte_pos++;//переходим к следующему байту
    				ir_tx_buffer_cursor.bit_mask = (1<<7); //старший бит уходит первым

    			}
    			if (tx_buffer[ir_tx_buffer_cursor.byte_pos]&ir_tx_buffer_cursor.bit_mask)//если текущий бит равен "1"
    			{
    				//timer3.TIM_Period = IR_ONE_BIT_DURATION + IR_SPACE_DURATION;
    				TIM8->ARR = IR_ONE_BIT_DURATION + IR_SPACE_DURATION;
    				timer8_PWM.TIM_Pulse = IR_ONE_BIT_DURATION;
    				//ir_pulse_counter = IR_ONE;//отправим "1" (помигаем 1200 микросекунд)
    			}
    			else //текущий бит равен "0"
    			{
    				//timer3.TIM_Period = IR_ZERO_BIT_DURATION + IR_SPACE_DURATION;
    				TIM8->ARR = IR_ZERO_BIT_DURATION + IR_SPACE_DURATION;
    				timer8_PWM.TIM_Pulse = IR_ZERO_BIT_DURATION;
    				//				ir_pulse_counter = IR_ZERO;//отправим "0" (помигаем 600 микросекунд)
    			}
    //			TIM_TimeBaseInit(TIM3, &timer3);
    			TIM_OC1Init(TIM8, (TIM_OCInitTypeDef*)&timer8_PWM);//применяем новые настройки периода и пулься
    //			ir_space_counter = IR_SPACE;      //и про паузу не забудем
    			ir_tx_buffer_cursor.bit_mask = (ir_tx_buffer_cursor.bit_mask >> 1); //следующий бит
    			ir_tx_buffer_cursor.bits_for_tx--; //уменьшаем количество оставшихся бит




    			//					ir_pulse_counter =data_packet.data[cursor_position++] ; //передадим импульс указанной длительностью

    		}
    		else //все биты передали
    		{
    			TIM1->CCR1 = 0; // включаем несущую ИК
    			TIM_ClearFlag(TIM8, TIM_SR_UIF/*TIM_DIER_UIE*//*TIM_FLAG_CC1OF*/);
    			TIM_Cmd(TIM8, DISABLE);//выключаем прерывание (передачу)
    			FLASH_LED_OFF;

    		}




    }
}

