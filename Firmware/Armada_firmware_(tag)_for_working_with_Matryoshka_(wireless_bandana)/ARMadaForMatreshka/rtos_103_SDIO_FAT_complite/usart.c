#include "stm32f10x.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "global_variables.h"

#ifndef bool

                        #define bool unsigned char

                        #define true 1

                        #define false 0

#endif

//передающий буфер
volatile unsigned char usart1_TxBuf[SIZE_BUF];
volatile unsigned char usart1_txBufTail = 0;
volatile unsigned char usart1_txBufHead = 0;
volatile unsigned char usart1_txCount = 0;

//приемный буфер
volatile unsigned char usart1_RxBuf[SIZE_BUF];
volatile unsigned char usart1_rxBufTail = 0;
volatile unsigned char usart1_rxBufHead = 0;
volatile unsigned char usart1_rxCount = 0;


volatile char usartData;
//extern volatile bool cr_received;//флаг, указывающий, что получен символ возврата корети - '\r'
/*******************************************************************/
//А в прерывании выдаем байт и увеличиваем счетчик
void USART1_IRQHandler()
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
		  xHigherPriorityTaskWoken = pdFALSE;

	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
      //  USART_SendData(USART1, usartData[usartCounter]);
      //  usartCounter++;
    	if (usart1_txCount > 0){              //если буфер не пустой
//    	    UDR = usartTxBuf[txBufHead]; //записываем в UDR символ из буфера
    	    USART_SendData(USART1, usart1_TxBuf[usart1_txBufHead]);//записываем в UDR символ из буфера
    	    usart1_txCount--;                   //уменьшаем счетчик символов
    	    usart1_txBufHead++;                 //инкрементируем индекс головы буфера
    	    if (usart1_txBufHead == SIZE_BUF) usart1_txBufHead = 0;
    	  }



    	USART_ClearITPendingBit(USART1, USART_IT_TC);

    }
//Проверяем, действительно ли прерывание вызвано приемом нового байта
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {

    	  if (usart1_rxCount < SIZE_BUF){                //если в буфере еще есть место
    	      usart1_RxBuf[usart1_rxBufTail] = USART_ReceiveData(USART1);        //считать символ из UDR в буфер
    	      if (GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_10)){//если блютус модль в режиме AT-команд
    	    	  if (usart1_RxBuf[usart1_rxBufTail]=='\r')
    	    	  {
    	    		  cr_received=true;
    	    		  xSemaphoreGiveFromISR( xUsart1Semaphore, &xHigherPriorityTaskWoken );

    	    	  }

    	      }
    	      else{//блютус модуль в режиме приёма данных
    	    	  if ((usart1_RxBuf[usart1_rxBufTail]=='t')||(usart1_RxBuf[usart1_rxBufTail]=='e')||(usart1_RxBuf[usart1_rxBufTail]=='f')||(usart1_RxBuf[usart1_rxBufTail]=='L'))//если таймаут или ошибка
    	    	  {
    	    	  	  xSemaphoreGiveFromISR( xBluetoothSemaphore, &xHigherPriorityTaskWoken );
    	    	  }

    	      }
    	      usart1_rxBufTail++;                             //увеличить индекс хвоста приемного буфера
    	      if (usart1_rxBufTail == SIZE_BUF) usart1_rxBufTail = 0;
    	      usart1_rxCount++;                                 //увеличить счетчик принятых символов
    	    }
//    USART_ClearITPendingBit(USART1, USART_IT_RXNE);

   // 	usartData = USART_ReceiveData(USART1);
    }
    if (USART1->SR & USART_SR_ORE)
    {
    	USART_ClearITPendingBit(USART1, USART_IT_ORE);
    }

}




/*******************************************************************/
//А в прерывании выдаем байт и увеличиваем счетчик
void USART2_IRQHandler()
{
    if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
    {
      //  USART_SendData(USART1, usartData[usartCounter]);
      //  usartCounter++;
    	USART_ClearITPendingBit(USART2, USART_IT_TC);

    }
//Проверяем, действительно ли прерывание вызвано приемом нового байта
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
	usartData = USART_ReceiveData(USART2);
	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }


}

void InitCOM1(uint32_t boudRate)
{
	GPIO_InitTypeDef port;
	USART_InitTypeDef usart;

//Включаем тактирование
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);
    //Пины PA9 и PA10 в режиме альтернативных функций –
     //Rx и Tx USART’а
    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_AF_PP;
    port.GPIO_Pin = GPIO_Pin_9;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);


    port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    port.GPIO_Pin = GPIO_Pin_10;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);

    //Настройка USART, все поля оставляем дефолтными, кроме скорости обмена
    USART_StructInit(&usart);
    usart.USART_BaudRate = boudRate;//BAUDRATE;
    USART_Init(USART1, &usart);


    //Включаем прерывания по приему байта и по окончанию передачи
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    //Запускаем сам USART
    USART_Cmd(USART1, ENABLE);
    //Разрешаем прерывания
    NVIC_SetPriority(USART1_IRQn, 12);
    NVIC_EnableIRQ(USART1_IRQn);


    /*

	NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(COM1_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

	if(COM1_GPIO_REMAP != 0)
	{
		GPIO_PinRemapConfig(COM1_GPIO_REMAP, ENABLE);
	}

	switch(COM1_CLK_APB)
	{
		case 1:
			RCC_APB1PeriphClockCmd(COM1_CLK, ENABLE);
			break;
		case 2:
		default:
			RCC_APB2PeriphClockCmd(COM1_CLK, ENABLE);
			break;
	}


	// TX как Alternate Function Push-Pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = COM1_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COM1_GPIO, &GPIO_InitStructure);

	// RX как Floating Input
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = COM1_RxPin;
	GPIO_Init(COM1_GPIO, &GPIO_InitStructure);

	// 115200 бод, 8 бит, 1 стоп, N четность, без контроля, разрешены прием и передача
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(COM1, &USART_InitStructure);

	// Конфигурация прерывания
	NVIC_InitStructure.NVIC_IRQChannel = COM1_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Включаем прерывание на получение
	USART_ITConfig(COM1, USART_IT_RXNE, ENABLE);


	USART_Cmd(COM1, ENABLE);
*/
}


void InitCOM2(uint32_t boudRate)
{
	GPIO_InitTypeDef port;
	USART_InitTypeDef usart;

//Включаем тактирование
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);
    //Пины PA2 и PA3 в режиме альтернативных функций –
     //Rx и Tx USART’а
    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_AF_PP;
    port.GPIO_Pin = GPIO_Pin_2;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);


    port.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    port.GPIO_Pin = GPIO_Pin_3;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);

    //Настройка USART, все поля оставляем дефолтными, кроме скорости обмена
    USART_StructInit(&usart);
    usart.USART_BaudRate = boudRate;//BAUDRATE;
    USART_Init(USART2, &usart);


    //Включаем прерывания по приему байта и по окончанию передачи
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    //Запускаем сам USART
    USART_Cmd(USART2, ENABLE);
    //Разрешаем прерывания
    NVIC_EnableIRQ(USART2_IRQn);


    /*

	NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(COM1_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

	if(COM1_GPIO_REMAP != 0)
	{
		GPIO_PinRemapConfig(COM1_GPIO_REMAP, ENABLE);
	}

	switch(COM1_CLK_APB)
	{
		case 1:
			RCC_APB1PeriphClockCmd(COM1_CLK, ENABLE);
			break;
		case 2:
		default:
			RCC_APB2PeriphClockCmd(COM1_CLK, ENABLE);
			break;
	}


	// TX как Alternate Function Push-Pull
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = COM1_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(COM1_GPIO, &GPIO_InitStructure);

	// RX как Floating Input
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = COM1_RxPin;
	GPIO_Init(COM1_GPIO, &GPIO_InitStructure);

	// 115200 бод, 8 бит, 1 стоп, N четность, без контроля, разрешены прием и передача
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(COM1, &USART_InitStructure);

	// Конфигурация прерывания
	NVIC_InitStructure.NVIC_IRQChannel = COM1_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Включаем прерывание на получение
	USART_ITConfig(COM1, USART_IT_RXNE, ENABLE);


	USART_Cmd(COM1, ENABLE);
*/
}

//______________________________________________________________________________
//возвращает колличество символов передающего буфера
unsigned char USART1_GetTxCount(void)
{
  return usart1_txCount;
}


//"очищает" передающий буфер
void USART1_FlushTxBuf(void)
{
	usart1_txBufTail = 0;
	usart1_txBufHead = 0;
	usart1_txCount = 0;
}

void USART1_PutChar(unsigned char sym) //положить символ в буфер
{
	 if (/*(USART_GetITStatus(USART1, USART_IT_TXE) == SET)*/(USART1->SR & USART_SR_TXE)&& (usart1_txCount == 0))
	 {
			USART_SendData(USART1, sym);
	 }
	 else {
		 if (usart1_txCount < SIZE_BUF){    //если в буфере еще есть место
			 usart1_TxBuf[usart1_txBufTail] = sym; //помещаем в него символ
			 usart1_txCount++;                   //инкрементируем счетчик символов
			 usart1_txBufTail++;                 //и индекс хвоста буфера
			 if (usart1_txBufTail == SIZE_BUF) usart1_txBufTail = 0;
			 }
		 }
}





//"очищает" приемный буфер
void USART1_FlushRxBuf(void)
{
  usart1_DisableRxInt();  //запрещаем прерывание по заверщению приема
  usart1_rxBufTail = 0;
  usart1_rxBufHead = 0;
  usart1_rxCount = 0;
  usart1_EnableRxInt();
}


//функция посылающая строку по usart`у
void USART1_SendStr(char * data)
{
volatile  unsigned char sym;
//  while(*data)
  while((sym = *data++)){

    USART1_PutChar(sym);
  }
}



//чтение буфера
unsigned char USART1_GetChar(void)
{
  unsigned char sym;
  if (usart1_rxCount > 0){                     //если приемный буфер не пустой
    sym = usart1_RxBuf[usart1_rxBufHead];        //прочитать из него символ
    usart1_rxCount--;                          //уменьшить счетчик символов
    usart1_rxBufHead++;                        //инкрементировать индекс головы буфера
    if (usart1_rxBufHead == SIZE_BUF) usart1_rxBufHead = 0;
    return sym;                         //вернуть прочитанный символ
  }
  return 0;
}



//______________________________________________________________________________
//возвращает колличество символов находящихся в приемном буфере
unsigned char USART1_GetRxCount(void)
{
  return usart1_rxCount;
}
