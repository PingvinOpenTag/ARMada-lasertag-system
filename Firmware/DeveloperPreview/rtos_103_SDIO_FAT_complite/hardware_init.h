#ifndef __HARDWARE_INIT_H
#define __HARDWARE_INIT_H

#include "stm32f10x.h"
#include "miles_protocol.h"
#include "global_variables.h"
#include "types.h"





void init_spi2(void)//настройка SPI2
{
	/** SPI2 GPIO Configuration
			 PB13	 ------> SPI2_SCK
			 PB15	 ------> SPI2_MOSI
		*/


		/*Enable or disable APB2 peripheral clock */
		GPIO_InitTypeDef GPIO_InitStruct;
		SPI_InitTypeDef SPIConf;


		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		/*Configure GPIO pin */
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		/* SS pin*/
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_SetBits(GPIOC, GPIO_Pin_3);

		// указываем, что используем мы только передачу данных
		    SPIConf.SPI_Direction = SPI_Direction_1Line_Tx;
		    // указываем, что наше устройство - Master
		    SPIConf.SPI_Mode = SPI_Mode_Master;
		    // передавать будем по 8 бит (=1 байт)
		    SPIConf.SPI_DataSize = SPI_DataSize_8b;
		    // режим 00
		    SPIConf.SPI_CPOL = SPI_CPOL_Low;
		    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;
		    SPIConf.SPI_NSS = SPI_NSS_Soft;
		    // установим скорость передачи (опытным путём выяснили, что разницы от изменения этого параметра нет)
		    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
		    // передаём данные старшим битом вперёд (т.е. слева направо)
		    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;
		    // внесём настройки в SPI
		    SPI_Init(SPI2, &SPIConf);
		    // включим  SPI1
		    SPI_Cmd(SPI2, ENABLE);
		    // SS = 1
		    SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Set);
			SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);


}


//таймер 6 подает сигналы каналу ДМА, чтобы тот считал очередной звуковой сеймпл
void init_timer(uint32_t sampleRate) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	//TIM2 - управление DAC (формирование синусоиды гармоники)
	  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	  TIM_TimeBaseStructure.TIM_Period = (uint16_t)(/*72000000*/SystemCoreClock/sampleRate);//1636;//1333;
	  TIM_TimeBaseStructure.TIM_Prescaler = 0;
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	  //TIM2 TRGO selection
	  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	  TIM_DMACmd(TIM6, TIM_DMA_Update, ENABLE);

////	  TIM_Cmd(TIM2, ENABLE);


/*
TIM_TimeBaseInitTypeDef tim_init;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
TIM_TimeBaseStructInit(&tim_init);
tim_init.TIM_RepetitionCounter = 0;
tim_init.TIM_Period = 1705 - 1;
tim_init.TIM_Prescaler = 0;
tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
tim_init.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM7, &tim_init);
TIM_SelectOutputTrigger(TIM7, TIM_TRGOSource_Update);
//TIM_Cmd(TIM4, ENABLE);
TIM_Cmd(TIM7, DISABLE);
//TIM_DMACmd();
*/

}





void init_dac(void) {
//DAC_InitTypeDef dac_init;
DAC_InitTypeDef DAC_InitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
/* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer =  DAC_OutputBuffer_Enable;// DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_1, ENABLE);
  DAC->DHR8R2=0;
 // DAC->DHR12L1 = 0;
  DAC->DHR12L1 = (uint16_t)0x8000;
  DAC_Cmd(DAC_Channel_2, ENABLE);
/*
DAC_StructInit(&dac_init);
dac_init.DAC_LFSRUnmask_TriangleAmplitude = 0;
dac_init.DAC_Trigger = DAC_Trigger_T7_TRGO;
dac_init.DAC_WaveGeneration = DAC_WaveGeneration_None;
//dac_init.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
dac_init.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
DAC_Init(DAC_Channel_1, &dac_init);
DAC_Cmd(DAC_Channel_1, ENABLE);
DAC_DMACmd(DAC_Channel_1, ENABLE);
*/

}
void DMA_for_ADC_Init(void){
    //enable DMA1 clock
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //create DMA structure
    DMA_InitTypeDef  DMA_InitStructure;
    //reset DMA1 channe1 to default values;
    DMA_DeInit(DMA1_Channel1);
    //channel will be used for memory to memory transfer
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    //setting normal mode (non circular)
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    //medium priority
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    //source and destination data size word=32bit
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    //automatic memory destination increment enable.
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //source address increment disable
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //Location assigned to peripheral register will be source
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //chunk of data to be transfered
    DMA_InitStructure.DMA_BufferSize = 4;
    //source and destination start addresses
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&ADC1->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_values;
    //send values to DMA registers
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    // Enable DMA1 Channel Transfer Complete interrupt
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1
/*
    NVIC_InitTypeDef NVIC_InitStructure;
    //Enable DMA1 channel IRQ Channel
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
*/

}
void init_adc_for_multiscan(void)//настройка АЦП для последовательного сканирования каналов
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 ;		// that's ADC1 IN1 (PA1 on STM32)
		GPIO_Init(GPIOA, &GPIO_InitStructure);
#ifndef OLD_VERSION_PCB
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 ;		// that's ADC1 IN2 (PС0 on STM32)
		GPIO_Init(GPIOC, &GPIO_InitStructure);
#else
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 ;		// that's ADC1 IN2 (PA2 on STM32)
		GPIO_Init(GPIOA, &GPIO_InitStructure);

#endif

		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 ;		// that's ADC1 IN12 (PС2 on STM32)
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		RCC_ADCCLKConfig (RCC_PCLK2_Div2);
	// enable ADC system clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		    //We will convert multiple channels
		ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		    //select continuous conversion mode
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//!
		    //select no external triggering
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		    //right 12-bit data alignment in ADC data register
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		    //8 channels conversion
		ADC_InitStructure.ADC_NbrOfChannel = 4;
		    //load structure values to control and status registers
		ADC_Init(ADC1, &ADC_InitStructure);
		//wake up temperature sensor
		ADC_TempSensorVrefintCmd(ENABLE);
		    //configure each channel
//PA1 - trigger/reload
		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_41Cycles5);
#ifndef OLD_VERSION_PCB
//PC0 - battary control
		ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_41Cycles5);
#else
//PA2 - battary control
		ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 2, ADC_SampleTime_41Cycles5);
#endif
//PC2 - fire mode switch
		ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_41Cycles5);
// internal Ref
		ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 4, ADC_SampleTime_41Cycles5);
//Enable ADC1
	    ADC_Cmd(ADC1, ENABLE);
	    //enable DMA for ADC
	    ADC_DMACmd(ADC1, ENABLE);
	    //Enable ADC1 reset calibration register
	    ADC_ResetCalibration(ADC1);
	    //Check the end of ADC1 reset calibration register
	    while(ADC_GetResetCalibrationStatus(ADC1));
	    //Start ADC1 calibration
	    ADC_StartCalibration(ADC1);
	    //Check the end of ADC1 calibration
	    while(ADC_GetCalibrationStatus(ADC1));
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void init_adc(void)//настройка АЦП
{

	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 ;		// that's ADC1 IN1 (PA1 on STM32)
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 ;		// that's ADC1 (PС2 on STM32)
		GPIO_Init(GPIOC, &GPIO_InitStructure);


		//clock for ADC (max 14MHz --> 72/6=12MHz)
//		RCC_ADCCLKConfig (RCC_PCLK2_Div6);
		RCC_ADCCLKConfig (RCC_PCLK2_Div2);
	// enable ADC system clock
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	// define ADC config
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	// we work in continuous sampling mode
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		ADC_InitStructure.ADC_NbrOfChannel = 1;


		ADC_RegularChannelConfig(ADC2,ADC_Channel_1, 1,ADC_SampleTime_28Cycles5); // define regular conversion config
		ADC_RegularChannelConfig(ADC3,ADC_Channel_12, 1,ADC_SampleTime_28Cycles5); // define regular conversion config
		ADC_RegularChannelConfig(ADC1,ADC_Channel_17, 1,ADC_SampleTime_28Cycles5); // define regular conversion config

		ADC_Init ( ADC1, &ADC_InitStructure);	//set config of ADC1
		ADC_Init ( ADC2, &ADC_InitStructure);	//set config of ADC2
		ADC_Init ( ADC3, &ADC_InitStructure);	//set config of ADC3

		ADC_TempSensorVrefintCmd(ENABLE);
/*
		NVIC_EnableIRQ(ADC1_2_IRQn);
		ADC_ITConfig(ADC2, ADC_IT_EOC, ENABLE);
		ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
*/

	// enable ADC
		ADC_Cmd (ADC1,ENABLE);	//enable ADC1
		ADC_Cmd (ADC2,ENABLE);	//enable ADC2
		ADC_Cmd (ADC3,ENABLE);	//enable ADC3

	//	ADC calibration (optional, but recommended at power on)
		ADC_ResetCalibration(ADC1);	// Reset previous calibration
		while(ADC_GetResetCalibrationStatus(ADC1));
		ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
		while(ADC_GetCalibrationStatus(ADC1));

	//	ADC calibration (optional, but recommended at power on)
		ADC_ResetCalibration(ADC2);	// Reset previous calibration
		while(ADC_GetResetCalibrationStatus(ADC2));
		ADC_StartCalibration(ADC2);	// Start new calibration (ADC must be off at that time)
		while(ADC_GetCalibrationStatus(ADC2));


	//	ADC calibration (optional, but recommended at power on)
		ADC_ResetCalibration(ADC3);	// Reset previous calibration
		while(ADC_GetResetCalibrationStatus(ADC3));
		ADC_StartCalibration(ADC3);	// Start new calibration (ADC must be off at that time)
		while(ADC_GetCalibrationStatus(ADC3));




	// start conversion
		ADC_Cmd (ADC1,ENABLE);	//enable ADC1
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);	// start conversion (will be endless as we are in continuous mode)


	// start conversion
		ADC_Cmd (ADC2,ENABLE);	//enable ADC2
		ADC_SoftwareStartConvCmd(ADC2, ENABLE);	// start conversion (will be endless as we are in continuous mode)


	// start conversion
		ADC_Cmd (ADC3,ENABLE);	//enable ADC3
		ADC_SoftwareStartConvCmd(ADC3, ENABLE);	// start conversion (will be endless as we are in continuous mode)


	// debug information
		RCC_ClocksTypeDef forTestOnly;
		RCC_GetClocksFreq(&forTestOnly);	//this could be used with debug to check to real speed of ADC clock







}


void init_pwm_TIM4(void){

	GPIO_InitTypeDef port;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_StructInit(&port);
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	port.GPIO_Pin = GPIO_Pin_6;
//	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &port);

	TIM_TimeBaseStructInit(&timer4);
	timer4.TIM_Prescaler = /*72*/(SystemCoreClock/1000000)-1;//считаем миллисекунды
////	    timer3.TIM_Period = IR_START_BIT_DURATION + IR_SPACE_DURATION;
	    TIM_TimeBaseInit(TIM4, &timer4);

	    TIM_ICInitTypeDef TIM_ICStructure;


	    TIM_ICStructure.TIM_Channel = TIM_Channel_1;//первый канал
	    TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // по заднему фронту
	    TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // прямо с ножки
	    TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // без делителя
	    TIM_ICStructure.TIM_ICFilter = 0; // без фильтра

	    // эта функция и включает режим PWM input - автоматически настраивает комплементарный канал
	              // правда в стандартной библиотеке работает на 1 и 2 канале, на 3 и 4 - не умеет
        TIM_PWMIConfig(TIM4, &TIM_ICStructure);

        /* Выбираем источник для триггера: вход 1 */
                 TIM_SelectInputTrigger(TIM4, TIM_TS_TI1FP1);
                 /* По событию от триггера счётчик будет сбрасываться. */
                 TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);
                 /* Включаем события от триггера */
                 TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);


                 // это третий канал, для таймаута. Таймаут 15мс, поскольку максимальный бит (старт) 13.5мс
                 TIM_OCInitTypeDef TIM_OCStructure;
                 TIM_OCStructure.TIM_OCMode = TIM_OCMode_Timing;
                 TIM_OCStructure.TIM_OutputState = TIM_OutputState_Disable;
                 TIM_OCStructure.TIM_OutputNState = TIM_OutputNState_Disable;
                 TIM_OCStructure.TIM_Pulse = 15000;
                 TIM_OC3Init(TIM4, &TIM_OCStructure);


                 /* Разрешаем таймеру генерировать прерывание по захвату */
                 TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
                 // и по таймауту третьего канала
                 TIM_ITConfig(TIM4, TIM_IT_CC3, DISABLE/*ENABLE*/);

                 TIM_ClearFlag(TIM4, TIM_FLAG_CC1);
                 TIM_ClearFlag(TIM4, TIM_FLAG_CC3);

                 /* Включаем таймер */
                 TIM_Cmd(TIM4, ENABLE);
                 //устанавливаем приоритет, долен быть больше 11 для корректной работы FreeRTOS
                 NVIC_SetPriority(TIM4_IRQn, 12);
                  // разрешаем прерывания
                 NVIC_EnableIRQ(TIM4_IRQn);
}

void init_pwm_TIM3(void){

	GPIO_InitTypeDef port;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	GPIO_StructInit(&port);
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	port.GPIO_Pin = GPIO_Pin_6;
//	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &port);

	TIM_TimeBaseStructInit(&timer3);
	timer3.TIM_Prescaler = /*72*/(SystemCoreClock/1000000)-1;//считаем миллисекунды
////	    timer3.TIM_Period = IR_START_BIT_DURATION + IR_SPACE_DURATION;
	    TIM_TimeBaseInit(TIM3, &timer3);

	    TIM_ICInitTypeDef TIM_ICStructure;


	    TIM_ICStructure.TIM_Channel = TIM_Channel_1;//первый канал
	    TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // по заднему фронту
	    TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // прямо с ножки
	    TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // без делителя
	    TIM_ICStructure.TIM_ICFilter = 0; // без фильтра

	    // эта функция и включает режим PWM input - автоматически настраивает комплементарный канал
	              // правда в стандартной библиотеке работает на 1 и 2 канале, на 3 и 4 - не умеет
        TIM_PWMIConfig(TIM3, &TIM_ICStructure);

        /* Выбираем источник для триггера: вход 1 */
                 TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
                 /* По событию от триггера счётчик будет сбрасываться. */
                 TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
                 /* Включаем события от триггера */
                 TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);


                 // это третий канал, для таймаута. Таймаут 15мс, поскольку максимальный бит (старт) 13.5мс
                 TIM_OCInitTypeDef TIM_OCStructure;
                 TIM_OCStructure.TIM_OCMode = TIM_OCMode_Timing;
                 TIM_OCStructure.TIM_OutputState = TIM_OutputState_Disable;
                 TIM_OCStructure.TIM_OutputNState = TIM_OutputNState_Disable;
                 TIM_OCStructure.TIM_Pulse = 15000;
                 TIM_OC3Init(TIM3, &TIM_OCStructure);


                 /* Разрешаем таймеру генерировать прерывание по захвату */
                 TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
                 // и по таймауту третьего канала
                 TIM_ITConfig(TIM3, TIM_IT_CC3, DISABLE /*ENABLE*/);

                 TIM_ClearFlag(TIM3, TIM_FLAG_CC1);
                 TIM_ClearFlag(TIM3, TIM_FLAG_CC3);

                 /* Включаем таймер */
                 TIM_Cmd(TIM3, ENABLE);
                 //устанавливаем приоритет, долен быть больше 11 для корректной работы FreeRTOS
                 NVIC_SetPriority(TIM3_IRQn, 12);
                 // разрешаем прерывания
                 NVIC_EnableIRQ(TIM3_IRQn);


}

void init_pwm(void){

	GPIO_InitTypeDef port;
	TIM_TimeBaseInitTypeDef timer;
	TIM_OCInitTypeDef timerPWM;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);


	GPIO_StructInit(&port);
	port.GPIO_Mode = GPIO_Mode_AF_PP;
	port.GPIO_Pin = GPIO_Pin_8;
	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &port);

	//GPIO_ResetBits(GPIOA, GPIO_Pin_8);
//	GPIO_SetBits(GPIOA, GPIO_Pin_8);

	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = /*7200-1;*/(SystemCoreClock/(/*36000*/56000*100))-1;
	    timer.TIM_Period = 100;
	    TIM_TimeBaseInit(TIM1, &timer);

	    TIM_OCStructInit(&timerPWM);
	    timerPWM.TIM_Pulse = 0;//50;
	    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
	    timerPWM.TIM_OutputState = TIM_OutputState_Enable;
	    TIM_OC1Init(TIM1, &timerPWM);

	    TIM_BDTRInitTypeDef bdtr;
	      TIM_BDTRStructInit(&bdtr);
	      bdtr.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	      TIM_BDTRConfig(TIM1, &bdtr);


	    TIM_Cmd(TIM1, ENABLE);
}



void init_pwm_TIM2(void){

	GPIO_InitTypeDef port;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	GPIO_StructInit(&port);
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	port.GPIO_Pin = GPIO_Pin_15;
//	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &port);

	/*Configure peripheral I/O remapping */
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);

	TIM_TimeBaseStructInit(&timer2);
	timer2.TIM_Prescaler = /*72*/(SystemCoreClock/1000000)-1;//считаем миллисекунды
////	    timer3.TIM_Period = IR_START_BIT_DURATION + IR_SPACE_DURATION;
	    TIM_TimeBaseInit(TIM2, &timer2);

	    TIM_ICInitTypeDef TIM_ICStructure;


	    TIM_ICStructure.TIM_Channel = TIM_Channel_1;//первый канал
	    TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // по заднему фронту
	    TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // прямо с ножки
	    TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // без делителя
	    TIM_ICStructure.TIM_ICFilter = 0; // без фильтра

	    // эта функция и включает режим PWM input - автоматически настраивает комплементарный канал
	              // правда в стандартной библиотеке работает на 1 и 2 канале, на 3 и 4 - не умеет
        TIM_PWMIConfig(TIM2, &TIM_ICStructure);

        /* Выбираем источник для триггера: вход 1 */
                 TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);
                 /* По событию от триггера счётчик будет сбрасываться. */
                 TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
                 /* Включаем события от триггера */
                 TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);


                 // это третий канал, для таймаута. Таймаут 15мс, поскольку максимальный бит (старт) 13.5мс
                 TIM_OCInitTypeDef TIM_OCStructure;
                 TIM_OCStructure.TIM_OCMode = TIM_OCMode_Timing;
                 TIM_OCStructure.TIM_OutputState = TIM_OutputState_Disable;
                 TIM_OCStructure.TIM_OutputNState = TIM_OutputNState_Disable;
                 TIM_OCStructure.TIM_Pulse = 15000;
                 TIM_OC3Init(TIM2, &TIM_OCStructure);


                 /* Разрешаем таймеру генерировать прерывание по захвату */
                 TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
                 // и по таймауту третьего канала
                 TIM_ITConfig(TIM2, TIM_IT_CC3, DISABLE/*ENABLE*/);

                 TIM_ClearFlag(TIM2, TIM_FLAG_CC1);
                 TIM_ClearFlag(TIM2, TIM_FLAG_CC3);

                 /* Включаем таймер */
                 TIM_Cmd(TIM2, ENABLE);
                 // разрешаем прерывания
                 //устанавливаем приоритет, долен быть больше 11 для корректной работы FreeRTOS
                 NVIC_SetPriority(TIM2_IRQn, 12);
                 NVIC_EnableIRQ(TIM2_IRQn);

}




void init_pwm_TIM5(void){

	GPIO_InitTypeDef port;

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	GPIO_StructInit(&port);
	port.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	port.GPIO_Pin = GPIO_Pin_0;
//	port.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &port);

	TIM_TimeBaseStructInit(&timer5);
	timer5.TIM_Prescaler = /*72*/(SystemCoreClock/1000000)-1;//считаем миллисекунды
////	    timer3.TIM_Period = IR_START_BIT_DURATION + IR_SPACE_DURATION;
	    TIM_TimeBaseInit(TIM5, &timer5);

	    TIM_ICInitTypeDef TIM_ICStructure;


	    TIM_ICStructure.TIM_Channel = TIM_Channel_1;//первый канал
	    TIM_ICStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; // по заднему фронту
	    TIM_ICStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // прямо с ножки
	    TIM_ICStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // без делителя
	    TIM_ICStructure.TIM_ICFilter = 0; // без фильтра

	    // эта функция и включает режим PWM input - автоматически настраивает комплементарный канал
	              // правда в стандартной библиотеке работает на 1 и 2 канале, на 3 и 4 - не умеет
        TIM_PWMIConfig(TIM5, &TIM_ICStructure);

        /* Выбираем источник для триггера: вход 1 */
                 TIM_SelectInputTrigger(TIM5, TIM_TS_TI1FP1);
                 /* По событию от триггера счётчик будет сбрасываться. */
                 TIM_SelectSlaveMode(TIM5, TIM_SlaveMode_Reset);
                 /* Включаем события от триггера */
                 TIM_SelectMasterSlaveMode(TIM5, TIM_MasterSlaveMode_Enable);


                 // это третий канал, для таймаута. Таймаут 15мс, поскольку максимальный бит (старт) 13.5мс
                 TIM_OCInitTypeDef TIM_OCStructure;
                 TIM_OCStructure.TIM_OCMode = TIM_OCMode_Timing;
                 TIM_OCStructure.TIM_OutputState = TIM_OutputState_Disable;
                 TIM_OCStructure.TIM_OutputNState = TIM_OutputNState_Disable;
                 TIM_OCStructure.TIM_Pulse = 15000;
                 TIM_OC3Init(TIM5, &TIM_OCStructure);


                 /* Разрешаем таймеру генерировать прерывание по захвату */
                 TIM_ITConfig(TIM5, TIM_IT_CC1, ENABLE);
                 // и по таймауту третьего канала
                 TIM_ITConfig(TIM5, TIM_IT_CC3, /*ENABLE*/ DISABLE);
                 //сбросим флаг переполнения третьего канала
                 TIM_ClearITPendingBit(TIM5, TIM_IT_CC3);

                 TIM_ClearFlag(TIM5, TIM_FLAG_CC1);
                 TIM_ClearFlag(TIM5, TIM_FLAG_CC3);

                 /* Включаем таймер */
                 TIM_Cmd(TIM5, ENABLE);
                 //устанавливаем приоритет, долен быть больше 11 для корректной работы FreeRTOS
                 NVIC_SetPriority(TIM5_IRQn, 12);
                 // разрешаем прерывания
                 NVIC_EnableIRQ(TIM5_IRQn);
}




void init_TIM7(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	//Заполняем поля структуры дефолтными значениями
	TIM_TimeBaseStructInit(&timer7);
	 //Выставляем предделитель
#ifdef SENSORS_BECKLIGHT
	timer7.TIM_Prescaler = 2*/*7200*/((SystemCoreClock/NUMBER_OF_SENSORS_FRAMES)/10000)-1;
#else
	timer7.TIM_Prescaler = 2*/*7200*/(SystemCoreClock/10000)-1;
#endif
	//Тут значение, досчитав до которого таймер сгенерирует прерывание
	timer7.TIM_Period = 100;
	//Инициализируем TIM4 нашими значениями
	TIM_TimeBaseInit(TIM7, &timer7);
	//Настраиваем таймер для генерации прерывания по обновлению (переполнению)
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
    //устанавливаем приоритет, долен быть больше 11 для корректной работы FreeRTOS
    NVIC_SetPriority(TIM7_IRQn, 14);
	//Запускаем таймер
	TIM_Cmd(TIM7, ENABLE);
	//Разрешаем соответствующее прерывание
	NVIC_EnableIRQ(TIM7_IRQn);
	//NVIC_EnableIRQ( TIM7_UP_IRQn);

}



void init_pwm_TIM8(void){

//	GPIO_InitTypeDef port;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

//	GPIO_StructInit(&port);
//	port.GPIO_Mode = GPIO_Mode_AF_PP;
//	port.GPIO_Pin = GPIO_Pin_6;
//	port.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_Init(GPIOB, &port);

	TIM_TimeBaseStructInit(&timer8);
	timer8.TIM_Prescaler = /*72*/(SystemCoreClock/1000000)-1;//считаем миллисекунды
	    timer8.TIM_Period = IR_START_BIT_DURATION + IR_SPACE_DURATION;
	    TIM_TimeBaseInit(TIM8, &timer8);

	    TIM_OCStructInit(&timer8_PWM);
	    timer8_PWM.TIM_Pulse = IR_START_BIT_DURATION;
	    timer8_PWM.TIM_OCMode = TIM_OCMode_PWM1;
	    timer8_PWM.TIM_OutputState = TIM_OutputState_Disable;
	    TIM_OC1Init(TIM8, &timer8_PWM);
	    TIM_ITConfig(TIM8, TIM_IT_CC1, ENABLE);//разрешаем прерывания по захвату/сравнению
//	    TIM_ITConfig(TIM3, TIM_FLAG_CC1OF, ENABLE);//разрешаем прерывания по захвату/сравнению
	    TIM_ITConfig(TIM8, TIM_DIER_UIE, ENABLE);//разрешаем прерывания по захвату/сравнению

	    TIM_Cmd(TIM8, ENABLE);
	    NVIC_EnableIRQ( TIM8_UP_IRQn);
	    NVIC_EnableIRQ( TIM8_CC_IRQn);
}








#endif
