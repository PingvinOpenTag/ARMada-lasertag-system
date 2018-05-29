#include "si4432.h"

#ifdef SI4432_ENABLE

void Delay_us(unsigned int  n)
{
        u32 i;

        while(n--)
        {
           i=2;
           while(i--);
  }
}

void init_gpio_for_si4432(void){

//if ((SPI_TypeDef*)SI4432_SPI==SPI2){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =SI4432_nSEL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SI4432_nSEL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =SI4432_SDN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SI4432_SDN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =SI4432_IQR_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SI4432_IQR_PORT, &GPIO_InitStructure);

//}[if ((SPI_TypeDef*)SI4432_SPI==SPI2)]


	SI4432_nSEL_RCC_APB_PeriphClockCmd(SI4432_nSEL_RCC_APB_Periph_GPIO	,ENABLE);
	SI4432_SDN_RCC_APB_PeriphClockCmd(SI4432_SDN_RCC_APB_Periph_GPIO,ENABLE);
	SI4432_IQR_RCC_APB_PeriphClockCmd(SI4432_IQR_RCC_APB_Periph_GPIO,ENABLE);

	SI4432_nSEL_HIGH;
	SI4432_SDN_LOW;
	Delay_us(15000);

}



void init_isr_for_si4432(void){
#if SI4432_SPI_NUMBER==2
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;
	GPIO_EXTILineConfig(SI4432_IQR_GPIO_PortSource, SI4432_IQR_GPIO_PinSource);
	exti.EXTI_Line = SI4432_IQR_EXTI_Line;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Falling;//по спаду
	exti.EXTI_LineCmd = ENABLE;

	EXTI_Init(&exti);


	/* Enable and set EXTI9_5 Interrupt to the lowest priority */
		nvic.NVIC_IRQChannel = SI4432_IQR_EXTI_IRQn;
		nvic.NVIC_IRQChannelPreemptionPriority = 14;
		nvic.NVIC_IRQChannelSubPriority = 14;
		nvic.NVIC_IRQChannelCmd = ENABLE;

		NVIC_Init(&nvic);
#endif
}



void init_spi_for_si4432(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPIConf;
	/** SPI2 GPIO Configuration
		 PB13	 ------> SPI2_SCK
		 PB15	 ------> SPI2_MOSI
		 PB14	 ------> SPI2_MISO
	*/
	/** SPI3 GPIO Configuration
			 PB3	 ------> SPI2_SCK
			 PB5	 ------> SPI2_MOSI
			 PB4	 ------> SPI2_MISO
		*/

	init_gpio_for_si4432();
    SPI_Cmd(SI4432_SPI, DISABLE);
	/*Enable or disable APB2 peripheral clock */
	SPI_I2S_DeInit(SI4432_SPI);

	if ((SPI_TypeDef*)SI4432_SPI==SPI2){

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		/*Configure GPIO pin */
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);

	}
	else if ((SPI_TypeDef*)SI4432_SPI==SPI3){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
		/*Configure GPIO pin */
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
	} else return;


	// указываем, что используем мы только передачу данных
	    SPIConf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;// SPI_Direction_1Line_Tx;
	    // указываем, что наше устройство - Master
	    SPIConf.SPI_Mode = SPI_Mode_Master; //	S.SPI_Mode = SPI_Mode_Master;
	    // передавать будем по 8 бит (=1 байт)
	    SPIConf.SPI_DataSize = SPI_DataSize_8b; //	S.SPI_DataSize = SPI_DataSize_8b;
	    // режим 00
	    SPIConf.SPI_CPOL = SPI_CPOL_Low;//	S.SPI_CPOL = SPI_CPOL_Low;
	    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;//	S.SPI_CPHA = SPI_CPHA_1Edge;
	    SPIConf.SPI_NSS = SPI_NSS_Soft;//S.SPI_NSS = SPI_NSS_Soft;
	    // установим скорость передачи (опытным путЄм вы€снили, что разницы от изменени€ этого параметра нет)
	    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//		 S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

	    // передаЄм данные старшим битом вперЄд (т.е. слева направо)
	    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;//	S.SPI_FirstBit = SPI_FirstBit_MSB;
	    SPIConf.SPI_CRCPolynomial=10;
	// внесЄм настройки в SPI
	    SPI_Init(SI4432_SPI, &SPIConf);
	    // включим  SPI1
	    SPI_Cmd(SI4432_SPI, ENABLE);

	    // ѕоскольку сигнал NSS контролируетс€ программно, установим его в единицу
	    // ≈сли сбросить его в ноль, то наш SPI модуль подумает, что
	    // у нас мультимастерна€ топологи€ и его лишили полномочий мастера.

	    // SS = 1
	    SPI_NSSInternalSoftwareConfig(SI4432_SPI, SPI_NSSInternalSoft_Set);
		SPI_I2S_DMACmd(SI4432_SPI, SPI_I2S_DMAReq_Tx, DISABLE);

}


/*******************************************************************************
* Function Name   : SPI_RW
* Description : Sends a byte through the SPI interface and return the byte
*                received from the SPI bus.
* Input       : byte : byte to send.
* Output       : None
* Return       : The value of the received byte.
*******************************************************************************/
u8 si4432_spi_readWrite(u8 byte)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SI4432_SPI, SPI_I2S_FLAG_TXE) == RESET);
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SI4432_SPI, byte);
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SI4432_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SI4432_SPI);
}

unsigned char si4432_spi_readWriteReg(unsigned char reg, unsigned char value)
{
        unsigned char status;
        SI4432_nSEL_LOW;
        // select register
        status = si4432_spi_readWrite(reg);
        // set value
        si4432_spi_readWrite(value);
        SI4432_nSEL_HIGH;
        return(status);
}



unsigned char si4432_spi_readReg(unsigned char reg)
{
    unsigned char reg_val;
    SI4432_nSEL_LOW;
    si4432_spi_readWrite(reg);
	reg_val = si4432_spi_readWrite(0xAA);
    SI4432_nSEL_HIGH;
	return(reg_val);
}



void si4432_spi_writeReg(unsigned char reg, unsigned char value)
{
        SI4432_nSEL_LOW;
        // select register
        si4432_spi_readWrite(reg| 0x80);
        // set value
        si4432_spi_readWrite(value);
        SI4432_nSEL_HIGH;
}



void si4432_spi_readBuf(unsigned char reg,unsigned char *pBuf, unsigned char bytes)
{
        unsigned char i;
        SI4432_nSEL_LOW;
        // Select register to write to and read status byte
        si4432_spi_readWrite(reg);
        for(i=0;i<bytes;i++)
        	pBuf[i] = si4432_spi_readWrite(0xAA);
        SI4432_nSEL_HIGH;
}

void si4432_spi_writeBuf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
        unsigned char i;
        SI4432_nSEL_LOW;
        // Select register to write to and read status byte
        si4432_spi_readWrite(reg| 0x80);
        for(i=0; i<bytes; i++) // then write all byte in buffer(*pBuf)
        	si4432_spi_readWrite(*pBuf++);
        SI4432_nSEL_HIGH;
}


//extern void si4432_IQR_callback(void);

#if SI4432_SPI_NUMBER==2

void EXTI0_IRQHandler(void){
	si4432_IQR_callback();
	EXTI_ClearFlag(SI4432_IQR_EXTI_Line);
}
#endif


#endif
