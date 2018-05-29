#include "types.h"

#ifdef DISPLAY_ENABLE
#include <ILI9163.h>
#include "f10x-pcd8544.h"

#ifdef COLOR_LCD
extern volatile uint32_t MSec;

void Delay(uint32_t MS){
	vTaskDelay(MS/(1000/TIC_FQR));
//	volatile uint32_t MSS = MSec;
//	while((MSec - MSS)<MS) __asm volatile ("nop");
}

//RGB222 to RGB565
uint16_t EToS(uint8_t Col){
	uint16_t Temp = 0;

	/* 8 bit
	Temp |= (Col&3)<<3;
	Temp |= ((Col>>2)&7)<<8;
	Temp |= (Col>>5)<<13;
	 */

	Temp |= (Col&3)<<3;
	Temp |= ((Col>>2)&3)<<9;
	Temp |= ((Col>>4)&3)<<14;

	return Temp;
}

void SB(uint8_t Data, uint8_t DR){
	if(DR == Dat) 	LCD_DC1;            // передача данных //GPIO_SetBits(GPIOA, AOPin);
	else LCD_DC0;//GPIO_ResetBits(GPIOA, AOPin);

	SPI3Send((uint16_t)Data);//SPI_SendData8(SPI1, Data);
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
}

void SW(uint16_t Data, uint8_t DR){
	if(DR == Dat) 	LCD_DC1;            // передача данных //GPIO_SetBits(GPIOA, AOPin);
	else LCD_DC0;//GPIO_ResetBits(GPIOA, AOPin);

	SPI3Send((uint16_t)(Data>>8));//SPI_SendData8(SPI1, Data>>8);
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	SPI3Send((uint16_t)Data);
//	SPI_SendData8(SPI1, Data);
//	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
}


void SetAddr(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2){
	SB(0x2A, Reg);
	SB(0x00, Dat);
#ifdef LCD_NOT_STANDART
	SB(X1+2, Dat);
#else
	SB(X1, Dat);
#endif

	SB(0x00, Dat);

#ifdef LCD_NOT_STANDART
	SB(X2+2, Dat);
#else
	SB(X2, Dat);
#endif
	SB(0x2B, Reg);
	SB(0x00, Dat);
#ifdef LCD_NOT_STANDART
	SB(Y1+3, Dat);
#else
	SB(32+Y1, Dat);
#endif
	SB(0x00, Dat);

#ifdef LCD_NOT_STANDART
	SB(Y2+3, Dat);
#else
	SB(32+Y2, Dat);
#endif
	SB(0x2C, Reg);
}


void SetAddrForBMP(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2){
	SB(0x2A, Reg);
	SB(0x00, Dat);
#ifdef LCD_NOT_STANDART
	SB(X1+2, Dat);
#else
	SB(X1, Dat);
#endif

	SB(0x00, Dat);

#ifdef LCD_NOT_STANDART
	SB(X2+2, Dat);
#else
	SB(X2, Dat);
#endif
	SB(0x2B, Reg);
	SB(0x00, Dat);
#ifdef LCD_NOT_STANDART
	SB(Y1+3, Dat);
#else
	SB(Y1, Dat);
#endif
	SB(0x00, Dat);

#ifdef LCD_NOT_STANDART
	SB(Y2+3, Dat);
#else
	SB(Y2, Dat);
#endif
	SB(0x2C, Reg);
}


void SetAddrForRotate90(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2){
	SB(0x2A, Reg);
	SB(0x00, Dat);
#ifdef LCD_NOT_STANDART
	SB(X1+2, Dat);
#else
	SB(32+X1, Dat);
#endif
	SB(0x00, Dat);
#ifdef LCD_NOT_STANDART
	SB(X2+2, Dat);
#else
	SB(32+X2, Dat);
#endif
	SB(0x2B, Reg);
	SB(0x00, Dat);
#ifdef LCD_NOT_STANDART
	SB(Y1+3, Dat);
#else
	SB(Y1, Dat);
#endif
	SB(0x00, Dat);
#ifdef LCD_NOT_STANDART
	SB(Y2+3, Dat);
#else
	SB(Y2, Dat);
#endif
	SB(0x2C, Reg);
}

void SetScrn(Colours8 Colour){
	uint16_t XCnt, YCnt;

	SetAddr(0, 0, XPix-1, YPix-1);

	for(XCnt = 0; XCnt<XPix; XCnt++){
		for(YCnt = 0; YCnt<YPix; YCnt++){
			SW(Colour, Dat);
		}
	}
}

void FillRect(Colours8 Colour, uint8_t X, uint8_t Y, uint8_t size_x, uint8_t size_y){
	uint16_t XCnt, YCnt;
	while (!(SPI3->SR & SPI_SR_TXE)); // Wait for bus free
		while (SPI3->SR & SPI_SR_BSY);
		init_spi3();
		SB(0x36, Reg); //Set Memory access mode
#if    		LCD_MODUL_VERSION == 2
		SB((0x08  |(1<<7)|(1<<6)), Dat);
#elif 	LCD_MODUL_VERSION == 1
		SB((0x08 /*|(1<<7)*/), Dat);
#endif
		SetAddr(0, 0, size_x-1, size_y-1);
		for(XCnt = 0; XCnt<size_x; XCnt++){
				for(YCnt = 0; YCnt<size_y; YCnt++){
					SW(Colour, Dat);
				}
			}

}

void ClrScrn(void){
	while (!(SPI3->SR & SPI_SR_TXE)); // Wait for bus free
	while (SPI3->SR & SPI_SR_BSY);
	init_spi3();
	SB(0x36, Reg); //Set Memory access mode
#if    		LCD_MODUL_VERSION == 2
	SB((0x08 |(1<<7)|(1<<6)), Dat);
#elif 	LCD_MODUL_VERSION == 1
	SB((0x08 /*|(1<<7)*/), Dat);
#endif
	SetScrn(BKGCol);
}

void WritePix(uint16_t X, uint16_t Y, Colours8 Colour){
	SetAddr(X, Y, X, Y);
	//PCol(Colour);
	SW(Colour, Dat);
}

void PCol(Colours8 Colour){
	/*
	switch(Colour){
	case Black:
		SW(0x0000, Dat);
		break;
	case Blue:
		SW(0x0010, Dat);
		break;
	case Red:
		SW(0x8000, Dat);
		break;
	case Magenta:
		SW(0x8010, Dat);
		break;
	case Green:
		SW(0x0400, Dat);
		break;
	case Cyan:
		SW(0x0410, Dat);
		break;
	case Yellow:
		SW(0x8400, Dat);
		break;
	case White:
		SW(0x8410, Dat);
		break;
	}
	*/
}

void SleepMode(uint8_t Mode){
	if(Mode == Sleep) SB(0x10, Reg);
	else SB(0x11, Reg);

	Delay(120);
}

void InvMode(uint8_t Mode){
	if(Mode==0) SB(0x20, Reg);
	else SB(0x21, Reg);
}




GPIO_InitTypeDef G;
SPI_InitTypeDef S;








void ILI9163Init(void){

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // SWD Remap (освобождаем PA15, PB3, PB4)
//   GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);


   RCC_APB2PeriphClockCmd(LCD_PH, ENABLE);

	  GPIO_InitTypeDef GPIO_InitStructure;

	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	  GPIO_InitStructure.GPIO_Pin   = /*SCLK | MOSI |*/ DC | RST | SCE ;
	  GPIO_Init(LCD_PORT, &GPIO_InitStructure);


	/*
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	G.GPIO_Pin = CSPin | ResPin | AOPin | BLPin | VCCPin;
	G.GPIO_Mode = GPIO_Mode_Out_PP;
	G.GPIO_OType = GPIO_OType_PP;
	G.GPIO_PuPd = GPIO_PuPd_NOPULL;
	G.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &G);

	G.GPIO_Pin = DatPin | ClkPin;
	G.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &G);

	GPIO_PinAFConfig(GPIOA, DatPS, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOA, ClkPS, GPIO_AF_0);

	S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	S.SPI_CPHA = SPI_CPHA_1Edge;
	S.SPI_CPOL = SPI_CPOL_Low;
	S.SPI_DataSize = SPI_DataSize_8b;
	S.SPI_FirstBit = SPI_FirstBit_MSB;
	S.SPI_Mode = SPI_Mode_Master;
	S.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1, &S);
	SPI_Cmd(SPI1, ENABLE);
*/

	init_spi3();

//	GPIO_ResetBits(GPIOA, ResPin);
	LCD_RST0;           // RST=0 - сброс дисплея

	Delay(20);
//	GPIO_SetBits(GPIOA, ResPin);
	LCD_RST1;           // RST=1
	Delay(20);

//	GPIO_ResetBits(GPIOA, CSPin);
	LCD_CS0;            // CS=0  - начали сеанс работы с дисплеем

//	GPIO_SetBits(GPIOA, BLPin);
	BL_ON;
	/*
	SB(0x11, Reg); //Exit sleep
	Delay(20);

	SB(0x26, Reg); //Set default gamma
	SB(0x04, Dat);

	SB(0xB1, Reg); //Set frame rate
	SB(0x0E, Dat);
	SB(0x10, Dat);

	SB(0xC0, Reg); //Set VRH1[4:0]...
	SB(0x08, Dat);
	SB(0x00, Dat);

	SB(0xC1, Reg);
	SB(0x05, Dat);
	SB(0xC5, Reg);
	SB(0x38, Dat);
	SB(0x40, Dat);

	SB(0x3A, Reg);
	SB(0x05, Dat);
	SB(0x36, Reg);
	SB(0x1C, Dat);

	SB(0x2A, Reg);
	SB(0x00, Dat);
	SB(0x00, Dat);
	SB(0x00, Dat);
	SB(0x7F, Dat);
	SB(0x2B, Reg);
	SB(0x00, Dat);
	SB(32, Dat);
	SB(0x00, Dat);
	SB(127+32, Dat);

	SB(0xB4, Reg);
	SB(0x00, Dat);

	SB(0xF2, Reg);
	SB(0x01, Dat);
	SB(0xE0, Reg);
	SB(0x3F, Dat);
	SB(0x22, Dat);
	SB(0x20, Dat);
	SB(0x30, Dat);
	SB(0x29, Dat);
	SB(0x0C, Dat);
	SB(0x4E, Dat);
	SB(0xB7, Dat);
	SB(0x3C, Dat);
	SB(0x19, Dat);
	SB(0x22, Dat);
	SB(0x1E, Dat);
	SB(0x02, Dat);
	SB(0x01, Dat);
	SB(0x00, Dat);
	SB(0xE1, Reg);
	SB(0x00, Dat);
	SB(0x1B, Dat);
	SB(0x1F, Dat);
	SB(0x0F, Dat);
	SB(0x16, Dat);
	SB(0x13, Dat);
	SB(0x31, Dat);
	SB(0x84, Dat);
	SB(0x43, Dat);
	SB(0x06, Dat);
	SB(0x1D, Dat);
	SB(0x21, Dat);
	SB(0x3D, Dat);
	SB(0x3E, Dat);
	SB(0x3F, Dat);

	SB(0x29, Reg);
	SB(0x2C, Reg);
	 */

	SB(0x01, Reg); //Software reset
	SB(0x11, Reg); //Exit Sleep

	Delay(20);

	SB(0x26, Reg); //Set default gamma
	SB(0x04, Dat);

	SB(0xC0, Reg); //Set Power Control 1
	SB(0x1F, Dat);

	SB(0xC1, Reg); //Set Power Control 2
	SB(0x00, Dat);

	SB(0xC2, Reg); //Set Power Control 3
	SB(0x00, Dat);
	SB(0x07, Dat);

	SB(0xC3, Reg); //Set Power Control 4 (Idle mode)
	SB(0x00, Dat);
	SB(0x07, Dat);

	SB(0xC5, Reg); //Set VCom Control 1
	SB(0x24, Dat); // VComH = 3v
	SB(0xC8, Dat); // VComL = 0v

	SB(0x38, Reg); //Idle mode off
	//SB(0x39, Reg); //Enable idle mode

	SB(0x3A, Reg); //Set pixel mode
	SB(0x05, Dat);


	/*
	MY, MX, MV, ML,RGB, MH, D1, D0
	0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 //normal
	1 | 0 | 0 | 0 | 1 | 0 | 0 | 0 //Y-Mirror
	0 | 1 | 0 | 0 | 1 | 0 | 0 | 0 //X-Mirror
	1 | 1 | 0 | 0 | 1 | 0 | 0 | 0 //X-Y-Mirror
	0 | 0 | 1 | 0 | 1 | 0 | 0 | 0 //X-Y Exchange
	1 | 0 | 1 | 0 | 1 | 0 | 0 | 0 //X-Y Exchange, Y-Mirror
	0 | 1 | 1 | 0 | 1 | 0 | 0 | 0 //XY exchange
	1 | 1 | 1 | 0 | 1 | 0 | 0 | 0
	*/


	SB(0x36, Reg); //Set Memory access mode
	SB((0x08/*|(1<<7)*/), Dat);

	SB(0x29, Reg); //Display on

	InvMode(0);
	ClrScrn();
}


void init_dma_to_color_lcd(uint16_t* pic_adress, uint32_t pic_size)//настраиваем DMA для работы с SPI2 (датчиками)
{
	DMA_InitTypeDef  DMA_to_SPI_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	DMA_DeInit ( DMA2_Channel2 );
	DMA_to_SPI_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(SPI3->DR);
	DMA_to_SPI_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pic_adress;//&Sinus;
	DMA_to_SPI_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_to_SPI_InitStructure.DMA_BufferSize = pic_size;
	DMA_to_SPI_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_to_SPI_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_to_SPI_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//DMA_PeripheralDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//DMA_MemoryDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;
	DMA_to_SPI_InitStructure.DMA_Priority = DMA_Priority_High;/*DMA_Priority_Low*/;
	DMA_to_SPI_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel2, &DMA_to_SPI_InitStructure);
	/* Разрешаем прерывание по окончанию передачи данных */

	DMA_ITConfig(DMA2_Channel2, DMA_IT_TC , ENABLE);
//	DMA_ITConfig(DMA2_Channel2, DMA_IT_HT , ENABLE);
//	DMA_ITConfig(DMA2_Channel2, DMA_IT_TE , ENABLE);



	NVIC_SetPriority(DMA2_Channel2_IRQn, 14);
	NVIC_EnableIRQ(DMA2_Channel2_IRQn);
	DMA_Cmd(DMA2_Channel2, DISABLE);
//	while ( !DMA_GetFlagStatus ( DMA2_FLAG_TC2 ) );

}



// обновление дисплея содержимым буфера
void color_lcd_dma_refresh(uint16_t data_size) {


	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);
	DMA_Cmd(DMA2_Channel2, DISABLE);
	while (!(SPI3->SR & SPI_SR_TXE)); // Wait for bus free
	while (SPI3->SR & SPI_SR_BSY);


	DMA_SetCurrDataCounter(DMA2_Channel2, data_size);
////	SPI_Cmd(SPI3, ENABLE);
	LCD_DC1;//данные
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA2_Channel2, ENABLE);


//	startSPI3();
/*
	unsigned char y, x;

	for (y=0;y<6;y++) for (x=0;x<84;x++) lcd8544_senddata(lcd8544_buff[y*84+x]);
*/
}


void init_spi3_to_color_lcd(void)//настройка SPI3
{
	/** SPI2 GPIO Configuration
			 PB13	 ------> SPI2_SCK
			 PB15	 ------> SPI2_MOSI
		*/


		/*Enable or disable APB2 peripheral clock */
		GPIO_InitTypeDef GPIO_InitStruct;
		SPI_InitTypeDef SPIConf;

		SPI_I2S_DeInit(SPI3);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
		/*Configure GPIO pin */
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStruct);
		/* SS pin*/
	/*
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_SetBits(GPIOC, GPIO_Pin_3);
*/



		// указываем, что используем мы только передачу данных
		    SPIConf.SPI_Direction = SPI_Direction_1Line_Tx;
		    // указываем, что наше устройство - Master
		    SPIConf.SPI_Mode = SPI_Mode_Master; //	S.SPI_Mode = SPI_Mode_Master;
		    // передавать будем по 8 бит (=1 байт)
		    SPIConf.SPI_DataSize = SPI_DataSize_16b; //	S.SPI_DataSize = SPI_DataSize_8b;
		    // режим 00
		    SPIConf.SPI_CPOL = SPI_CPOL_Low;//	S.SPI_CPOL = SPI_CPOL_Low;
		    SPIConf.SPI_CPHA = SPI_CPHA_1Edge;//	S.SPI_CPHA = SPI_CPHA_1Edge;
		    SPIConf.SPI_NSS = SPI_NSS_Soft;//S.SPI_NSS = SPI_NSS_Soft;
		    // установим скорость передачи (опытным путём выяснили, что разницы от изменения этого параметра нет)
		    SPIConf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//		 S.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;

		    // передаём данные старшим битом вперёд (т.е. слева направо)
		    SPIConf.SPI_FirstBit = SPI_FirstBit_MSB;//	S.SPI_FirstBit = SPI_FirstBit_MSB;
		    // внесём настройки в SPI
		    SPI_Init(SPI3, &SPIConf);//SPI_Init(SPI1, &S);
		    // включим  SPI1
		    SPI_Cmd(SPI3, ENABLE); //SPI_Cmd(SPI1, ENABLE);
		    // SS = 1
		    SPI_NSSInternalSoftwareConfig(SPI3, SPI_NSSInternalSoft_Set);

			SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);


}

#endif
#endif
