#include "types.h"
#ifdef DISPLAY_ENABLE
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "f10x-pcd8544.h"
#include "font6x8.h"	  // шрифт




unsigned char lcd8544_buff[84*6]; // буфер дисплея


// отправка данных\команд на дисплей
void lcd8544_senddata(unsigned char data) {
	SPI3Send((uint16_t)data);
	/*
	unsigned char i;
	for(i=0;i<8;i++) {
		if (data & 0x80)
		{
			LCD_MOSI1;
		}
		else
		{
			LCD_MOSI0;
		}
		data = data<<1;
		LCD_SCK0;
		int i = 0;
		while(i<100)
		{
			i++;
		}
		i--;
		LCD_SCK1;
		i = 0;
				while(i<100)
				{
					i++;
				}
		i--;
	}
*/
}

// обновление дисплея содержимым буфера
void lcd8544_refresh(void) {
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);
	DMA_Cmd(DMA2_Channel2, DISABLE);

	LCD_CS0;            // СS=0 - начали сеанс работы с дисплеем

	LCD_DC0;            // передача комманд
	lcd8544_senddata(0x40); // установка курсора в позицию Y=0; X=0
	lcd8544_senddata(0x80);

	LCD_DC1;            // передача данных
//	startSPI3();

	unsigned char y, x;

	for (y=0;y<6;y++) for (x=0;x<84;x++) lcd8544_senddata(lcd8544_buff[y*84+x]);

}

// обновление дисплея содержимым буфера
void lcd8544_dma_refresh(void) {


	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);
	DMA_Cmd(DMA2_Channel2, DISABLE);

	LCD_CS0;            // СS=0 - начали сеанс работы с дисплеем

	LCD_DC0;            // передача комманд
	lcd8544_senddata(0x40); // установка курсора в позицию Y=0; X=0
	lcd8544_senddata(0x80);

	LCD_DC1;            // передача данных
	while (!(SPI3->SR & SPI_SR_TXE)); // Wait for bus free
	while (SPI3->SR & SPI_SR_BSY);
////	SPI_Cmd(SPI3, DISABLE);
	DMA_SetCurrDataCounter(DMA2_Channel2,84*6+1);
////	SPI_Cmd(SPI3, ENABLE);
	SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA2_Channel2, ENABLE);


//	startSPI3();
/*
	unsigned char y, x;

	for (y=0;y<6;y++) for (x=0;x<84;x++) lcd8544_senddata(lcd8544_buff[y*84+x]);
*/
}
// Инициализация
void lcd8544_init(void) {

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // SWD Remap (освобождаем PA15, PB3, PB4)
//   GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);


   RCC_APB2PeriphClockCmd(LCD_PH, ENABLE);

	  GPIO_InitTypeDef GPIO_InitStructure;

	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	  GPIO_InitStructure.GPIO_Pin   = /*SCLK | MOSI |*/ DC | RST | SCE ;
	  GPIO_Init(LCD_PORT, &GPIO_InitStructure);

	  init_spi3();
	  init_dma_to_spi3();

	  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, DISABLE);
	  DMA_Cmd(DMA2_Channel2, DISABLE);

	  LCD_SCK0;
//	  GPIO_SetBits(GPIOB, GPIO_Pin_3);
	  // сброс дисплея
	  LCD_CS0;            // CS=0  - начали сеанс работы с дисплеем
	  LCD_RST0;           // RST=0 - сброс дисплея
	  LCD_RST1;           // RST=1

	  // последовательность инициализации дисплея
	  LCD_DC0;            // передача команды

	  lcd8544_senddata(0x21);      // переход в расширенный режим
	  lcd8544_senddata(0xC1);

	  lcd8544_senddata(0x06);		// температурный коэффициент, от 4 до 7

	  lcd8544_senddata(0x13);		// Bias 0b0001 0xxx - работает как контрастность

	  lcd8544_senddata(0x20); 	// переход в обычный режим
	  lcd8544_senddata(0b1100);	    // 0b1100 - normal mode
			                        // 0b1101 - invert mode
			                        // 0b1001 - полностью засвеченный экран
			                        // 0b1000 - чистый экран

	  LCD_DC1;            // передача команды

//	  lcd8544_refresh();
}

// вывод пиксела
void lcd8544_putpix(unsigned char x, unsigned char y, unsigned char mode) {
	if ((x>84) || (y>47)) return;
//
	unsigned int adr=(y>>3)*84+x;
	unsigned char data=(1<<(y&0x07));

	if (mode) lcd8544_buff[adr]|=data;
   else lcd8544_buff[adr]&=~data;
}



// процедура рисования линии
void lcd8544_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode) {
signed char   dx, dy, sx, sy;
unsigned char  x,  y, mdx, mdy, l;


  dx=x2-x1; dy=y2-y1;

  if (dx>=0) { mdx=dx; sx=1; } else { mdx=x1-x2; sx=-1; }
  if (dy>=0) { mdy=dy; sy=1; } else { mdy=y1-y2; sy=-1; }

  x=x1; y=y1;

  if (mdx>=mdy) {
     l=mdx;
     while (l>0) {
         if (dy>0) { y=y1+mdy*(x-x1)/mdx; }
            else { y=y1-mdy*(x-x1)/mdx; }
         lcd8544_putpix(x,y,mode);
         x=x+sx;
         l--;
     }
  } else {
     l=mdy;
     while (l>0) {
        if (dy>0) { x=x1+((mdx*(y-y1))/mdy); }
          else { x=x1+((mdx*(y1-y))/mdy); }
        lcd8544_putpix(x,y,mode);
        y=y+sy;
        l--;
     }
  }
  lcd8544_putpix(x2, y2, mode);

}



// рисование прямоугольника (не заполненного)
void lcd8544_rect(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char mode) {
	lcd8544_line(x1,y1, x2,y1, mode);
	lcd8544_line(x1,y2, x2,y2, mode);
	lcd8544_line(x1,y1, x1,y2, mode);
	lcd8544_line(x2,y1, x2,y2, mode);
}

// вывод символа на экран по координатам
void lcd8544_putchar(unsigned char px, unsigned char py, unsigned char ch, unsigned char mode) {
	const unsigned char *fontpointer;


	if (ch<127) {	// знакогенератор латиницы
		fontpointer=(const unsigned char*)NewFontLAT;
		ch=ch-32; }
		else	{	// знакогенератор русских букв
		fontpointer=NewFontRUS; ch=ch-192;}

    unsigned char lcd_YP=7- (py & 0x07);    // битовая позиция символа в байте
    unsigned char lcd_YC=(py & 0xF8)>>3; 	// байтовая позиция символа на экране
    unsigned char x;
	for (x=0; x<6; x++) {

		unsigned char temp=*(fontpointer+ch*6+x);

		if (mode!=0) {
			temp=~temp;
			if (py>0) lcd8544_putpix(px, py-1, 1);	// если печать в режиме инверсии - сверху отчертим линию
		}

		temp&=0x7F;

		lcd8544_buff[lcd_YC*84+px]=lcd8544_buff[lcd_YC*84+px] | (temp<<(7-lcd_YP)); 	// печать верхней части символа

	    if (lcd_YP<7) lcd8544_buff[(lcd_YC+1)*84+px]=lcd8544_buff[(lcd_YC+1)*84+px] | (temp>>(lcd_YP+1)); 	// печать нижней части символа

		px++;
		if (px>83) return;
	}
}


// печать десятичного числа
void lcd8544_dec(unsigned int numb, unsigned char dcount, unsigned char x, unsigned char y, unsigned char mode) {
	unsigned int divid=10000;
	unsigned char i;

	for (i=5; i!=0; i--) {

		unsigned char res=numb/divid;

		if (i<=dcount) {
			lcd8544_putchar(x, y, res+'0', mode);
			x=x+6;
		}

		numb%=divid;
		divid/=10;
	}
}


// линия левее символа для печати в инверсе
void lcd8544_leftchline(unsigned char x, unsigned char y) {
	if (x>0) lcd8544_line(x-1, y-1, x-1, y+6, 1);
}


// вывод строки
void lcd8544_putstr(unsigned char x, unsigned char y, const unsigned char str[], unsigned char mode) {
	if (mode) lcd8544_leftchline(x, y);
	while (*str!=0) {
		lcd8544_putchar(x, y, *str, mode);
		x=x+6;
		str++;
	}
}


void init_spi3(void)//настройка SPI3
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
		    SPIConf.SPI_DataSize = SPI_DataSize_8b; //	S.SPI_DataSize = SPI_DataSize_8b;
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


void SPI3Send(uint16_t data) {

	//GPIO_ResetBits(GPIOC, GPIO_Pin_3); // SS = 0
	SPI_I2S_SendData(SPI3, data);  // отправили данные
    while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET); // ждём, пока данные не отправятся
    //GPIO_SetBits(GPIOC, GPIO_Pin_3);
}


void init_dma_to_spi3(void)//настраиваем DMA для работы с SPI2 (датчиками)
{
	DMA_InitTypeDef  DMA_to_SPI_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	DMA_DeInit ( DMA2_Channel2 );
	DMA_to_SPI_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(SPI3->DR);
	DMA_to_SPI_InitStructure.DMA_MemoryBaseAddr = (uint32_t)lcd8544_buff;//&Sinus;
	DMA_to_SPI_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_to_SPI_InitStructure.DMA_BufferSize = 84*6;
	DMA_to_SPI_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_to_SPI_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_to_SPI_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;
	DMA_to_SPI_InitStructure.DMA_Priority = DMA_Priority_High;/*DMA_Priority_Low*/;
	DMA_to_SPI_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel2, &DMA_to_SPI_InitStructure);
	/* Разрешаем прерывание по окончанию передачи данных */

//	DMA_ITConfig(DMA2_Channel2, DMA_IT_TC , ENABLE);
//	DMA_ITConfig(DMA2_Channel2, DMA_IT_HT , ENABLE);
//	DMA_ITConfig(DMA2_Channel2, DMA_IT_TE , ENABLE);




	NVIC_EnableIRQ(DMA2_Channel2_IRQn);
	DMA_Cmd(DMA2_Channel2, DISABLE);
//	while ( !DMA_GetFlagStatus ( DMA2_FLAG_TC2 ) );

}


void startSPI3(void){
	DMA_Cmd(DMA2_Channel2, DISABLE);
	DMA_SetCurrDataCounter(DMA2_Channel2,84*6);
	DMA_Cmd(DMA2_Channel2, ENABLE);
}



// Draw a circle outline
void drawCircle(int16_t x0, int16_t y0, int16_t r, unsigned char mode) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  lcd8544_putpix((unsigned char) x0  , (unsigned char)(y0+r), (unsigned char)mode);
  lcd8544_putpix((unsigned char)x0  , (unsigned char)(y0-r), (unsigned char)mode);
  lcd8544_putpix((unsigned char)(x0+r),(unsigned char) y0  , (unsigned char)mode);
  lcd8544_putpix((unsigned char)(x0-r), (unsigned char)y0  , (unsigned char)mode);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    lcd8544_putpix((unsigned char)(x0 + x), (unsigned char)(y0 + y), (unsigned char)mode);
    lcd8544_putpix((unsigned char)(x0 - x), (unsigned char)(y0 + y), (unsigned char)mode);
    lcd8544_putpix((unsigned char)(x0 + x), (unsigned char)(y0 - y), (unsigned char)mode);
    lcd8544_putpix((unsigned char)(x0 - x), (unsigned char)(y0 - y), (unsigned char)mode);
    lcd8544_putpix((unsigned char)(x0 + y), (unsigned char)(y0 + x), (unsigned char)mode);
    lcd8544_putpix((unsigned char)(x0 - y), (unsigned char)(y0 + x), (unsigned char)mode);
    lcd8544_putpix((unsigned char)(x0 + y), (unsigned char)(y0 - x), (unsigned char)mode);
    lcd8544_putpix((unsigned char)(x0 - y), (unsigned char)(y0 - x), (unsigned char)mode);
  }
}



void drawFastVLine(int16_t x, int16_t y,
				 int16_t h, unsigned char mode) {
  // Update in subclasses if desired!
	lcd8544_line((unsigned char) x, (unsigned char)y, (unsigned char)x, (unsigned char)(y+h-1), (unsigned char)mode);
}

void drawFastHLine(int16_t x, int16_t y,
				 int16_t w, unsigned char mode) {
  // Update in subclasses if desired!
	lcd8544_line ((unsigned char)x, (unsigned char)y, (unsigned char)(x+w-1), y, (unsigned char)mode);
}


void drawCircleHelper( int16_t x0, int16_t y0,
               int16_t r, uint8_t cornername, unsigned char mode) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
    	lcd8544_putpix((unsigned char)(x0 + x), (unsigned char)(y0 + y), (unsigned char)mode);
    	lcd8544_putpix((unsigned char)(x0 + y), (unsigned char)(y0 + x), (unsigned char)mode);
    }
    if (cornername & 0x2) {
    	lcd8544_putpix((unsigned char)(x0 + x), (unsigned char)(y0 - y), (unsigned char)mode);
    	lcd8544_putpix((unsigned char)(x0 + y), (unsigned char)(y0 - x), (unsigned char)mode);
    }
    if (cornername & 0x8) {
    	lcd8544_putpix((unsigned char)(x0 - y),(unsigned char) (y0 + x), (unsigned char)mode);
    	lcd8544_putpix((unsigned char)(x0 - x), (unsigned char)(y0 + y), (unsigned char)mode);
    }
    if (cornername & 0x1) {
    	lcd8544_putpix((unsigned char)(x0 - y), (unsigned char)(y0 - x), (unsigned char)mode);
    	lcd8544_putpix((unsigned char)(x0 - x), (unsigned char)(y0 - y), (unsigned char)mode);
    }
  }
}



void drawBitmap(int16_t x, int16_t y,
			      const unsigned char *bitmap, uint8_t w, uint8_t h,
			      unsigned char mode) {

  uint8_t x_pos_in_bitmap;
  uint8_t y_pos_in_bitmap;
  for( x_pos_in_bitmap=0;  x_pos_in_bitmap<w;  x_pos_in_bitmap++) {
    for(y_pos_in_bitmap =0; y_pos_in_bitmap < h; y_pos_in_bitmap++ ) {
    	if(bitmap[x_pos_in_bitmap + ((y_pos_in_bitmap & 0xF8)>>3)*w] & (1<< (y_pos_in_bitmap& 0x07)))
    	{
    	  lcd8544_putpix((unsigned char)(x+ x_pos_in_bitmap), (unsigned char)(y+y_pos_in_bitmap),(unsigned char) mode);

    	}
    }
  }
}


void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
		unsigned char mode) {
  // Update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    drawFastVLine(i, y, h,mode);
  }
}

/*
void clearRect(int16_t x, int16_t y, int16_t w, int16_t h,
		unsigned char mode) {
  // Update in subclasses if desired!
  for (int16_t i=x; i<x+w; i++) {
    drawFastVLine(i, y, h,mode);
  }
}
*/

void drawDigit(int16_t x, int16_t y, char digit, unsigned char mode){

	if (mode)
	{
		 fillRect(x,y,12,16,0);
		 drawBitmap(x,y,(const unsigned char *)&alternative_digits[(unsigned char)digit][0],12,16,1);

	}
	else
	{
		fillRect(x,y,12,16,1);
		drawBitmap(x,y,(const unsigned char *)&alternative_digits[(unsigned char)digit][0],12,16,1);
	}
}

void drawSmallDigit(int16_t x, int16_t y, char digit, unsigned char mode){

	if (mode)
	{
		 fillRect(x,y,4,8,0);
		 drawBitmap(x,y,(const unsigned char *)&small_digits[(unsigned char)digit][0],4,8,1);

	}
	else
	{
		fillRect(x,y,4,8,1);
		drawBitmap(x,y,(const unsigned char *)&small_digits[(unsigned char)digit][0],4,8,1);
	}
}


void lcd_rounds_update(void){

	uint8_t thousands,hundreds,tens,units;
	uint8_t cursor_position = 0;
	uint8_t virtual_rounds;
	thousands = armadaSystem.gun.rounds/1000;
	hundreds = (armadaSystem.gun.rounds-1000*thousands)/100;
	tens = (armadaSystem.gun.rounds-1000*thousands- 100*hundreds )/10;
	units = armadaSystem.gun.rounds-1000*thousands- 100*hundreds - tens*10;
	fillRect(16,16+4,4*11-1,16,0);
	if(thousands)
	{
		drawDigit(16,16+4,thousands,1);
		cursor_position++;
	}
	if(hundreds)
	{
		drawDigit(16+cursor_position*12,16+4,hundreds,1);
		cursor_position++;
	}
	else {//сотни = 0
		if(cursor_position)//если тысячные выведены
		{
			drawDigit(16+cursor_position*12,16+4,0,1);//выводим 0
			cursor_position++;
		}
	}

	if(tens)
		{
			drawDigit(16+cursor_position*12,16+4,tens,1);
			cursor_position++;
		}
	else {//десятки=0
		if(cursor_position)//если тысячные или сотни выведены
				{
					drawDigit(16+cursor_position*12,16+4,0,1);//выводим 0
					cursor_position++;
				}
	}
	if(units)
	{
		drawDigit(16+cursor_position*12,16+4,units,1);
	}
	else{
		drawDigit(16+cursor_position*12,16+4,0,1);//выводим 0
	}
	virtual_rounds = (((armadaSystem.gun.rounds)*6)/armadaSystem.gun.rounds_placed_in_clip)+1;
	if(virtual_rounds>6)virtual_rounds=6;
	fillRect(0,16,16,24,0);
	if(virtual_rounds>0)
	{
		//fillRect(0,16,16,24,0);

		if (armadaSystem.gun.rounds>0)
		{
			drawBitmap(0,16,(const unsigned char*)&clip_img[virtual_rounds][0],16,24,1);
		}
		else{
			//fillRect(0,16,16,24,0);
			drawBitmap(0,16,(const unsigned char*)&clip_img[0][0],16,24,1);
		}
	}
	else{

		if (armadaSystem.gun.rounds>0)drawBitmap(0,16,(const unsigned char*)&clip_img[1][0],16,24,1);
		else drawBitmap(0,16,(const unsigned char*)&clip_img[0][0],16,24,1);

	}
	  if (!screen_auto_refresh)lcd8544_dma_refresh();
}


void lcd_clips_update(void){
	uint8_t thousands,hundreds,tens,units;
	uint8_t cursor_position = 0;
	thousands = armadaSystem.gun.clips/1000;
	hundreds = (armadaSystem.gun.clips-1000*thousands)/100;
	tens = (armadaSystem.gun.clips-1000*thousands- 100*hundreds )/10;
	units = armadaSystem.gun.clips-1000*thousands- 100*hundreds - tens*10;
	fillRect(0,41,4*5,16,0);

	if(thousands)
		{
			drawSmallDigit(0,41,thousands,1);
			cursor_position++;
		}
	if(hundreds)
		{
			drawSmallDigit(5*cursor_position,41,hundreds,1);
			cursor_position++;
		}
		else {//сотни = 0
			if(cursor_position)//если тысячные выведены
			{
				drawSmallDigit(5*cursor_position,41,0,1);
				cursor_position++;
			}
		}

		if(tens)
			{
				drawSmallDigit(5*cursor_position,41,tens,1);
				cursor_position++;
			}
		else {//десятки=0
			if(cursor_position)//если тысячные или сотни выведены
			{
				drawSmallDigit(5*cursor_position,41,0,1);//выводим 0
				cursor_position++;
			}
		}
		if(units)
		{
			drawSmallDigit(5*cursor_position,41,units,1);
		}
		else{
			drawSmallDigit(5*cursor_position,41,units,1);//выводим 0
		}
if (!screen_auto_refresh)lcd8544_dma_refresh();
}




void lcd_battary_voltage_update(uint16_t voltage){
	uint8_t integer,tenths,hundredths;
	integer = voltage/1000;//целая часть
	tenths = (voltage - integer*1000)/100;//десятые
	hundredths = (voltage - integer*1000 - tenths*100)/10;//сотые
	fillRect(84-16-1,8-1,4*5,4,0);
	drawSmallDigit(84-16-1,8-1,integer,1);
	drawSmallDigit(84-16+4-1,8,10,1);
	drawSmallDigit(84-16+8-1,8-1,tenths,1);
	drawSmallDigit(84-16+12,8-1,hundredths,1);

	fillRect(84-16+3,0+1,16-4,8-4,0);//удаляем изображение прогресс-бара в батарейке
	if ((armadaSystem.battary.full_voltage > armadaSystem.battary.low_voltage)&&(voltage > armadaSystem.battary.low_voltage))
	{
		uint16_t mV_in_step;
		int16_t x;
		mV_in_step = (armadaSystem.battary.full_voltage - armadaSystem.battary.low_voltage)/14;
		x = 84 - 1 - (voltage - armadaSystem.battary.low_voltage)/mV_in_step;
		if (x < 83)
		{
			fillRect(x,0+1,(84-1)-x,8-3,1);//рисуем изображение прогресс-бара в батарейке
		}
		//		drawBitmap(84-16,0, battary_img,16,8,1);
	}

if (!screen_auto_refresh)lcd8544_dma_refresh();

}



void lcd_fire_mode_update(tfire_mode fire_mode)
{
	fillRect(84-6*4-2,48-8,6*4,8,0);//удаляем изображение прогресс-бара в батарейке
			switch(fire_mode)
			{
				case SINGLE:
				{
					lcd8544_putstr(84-6*4-2,48-8 ,(const unsigned char *)"SING", 1); // вывод первой строки
				}
				break;
				case SEMI_AUTO:
				{
					lcd8544_putstr(84-6*4-2,48-8 ,(const unsigned char *)"SEMI", 1); // вывод первой строки
				}
				break;
				case AUTO:
				{
					lcd8544_putstr(84-6*4-2,48-8 ,(const unsigned char *)"AUTO", 1); // вывод первой строки
				}
				break;
				default: break;

			}


			  drawFastHLine(84-6*4-3,48-1,6*4+2,1);
			  drawFastVLine(84-1,48-8-1,9,1);
			  drawFastVLine(84-2,48-8-1,9,1);

if (!screen_auto_refresh)lcd8544_dma_refresh();
}

void lcd_health_update(void){

		uint8_t thousands,hundreds,tens,units;
		uint8_t cursor_position = 0;
		thousands = armadaSystem.player.health/1000;
		hundreds = (armadaSystem.player.health-1000*thousands)/100;
		tens = (armadaSystem.player.health-1000*thousands- 100*hundreds )/10;
		units = armadaSystem.player.health-1000*thousands- 100*hundreds - tens*10;
		fillRect(16,0,3*12,16,0);//удаляем изображение
		if(thousands)
			{
				drawDigit(16,0,thousands,1);
				cursor_position++;
			}
		if(hundreds)
			{
				drawDigit(16+12*cursor_position,0,hundreds,1);
				cursor_position++;
			}
			else {//сотни = 0
				if(cursor_position)//если тысячные выведены
				{
					drawDigit(16+12*cursor_position,0,0,1);
					cursor_position++;
				}
			}

			if(tens)
				{
					drawDigit(16+12*cursor_position,0,tens,1);
					cursor_position++;
				}
			else {//десятки=0
				if(cursor_position)//если тысячные или сотни выведены
				{
					drawDigit(16+12*cursor_position,0,0,1);//выводим 0
					cursor_position++;
				}
			}
			if(units)
			{
				drawDigit(16+12*cursor_position,0,units,1);
			}
			else{
				drawDigit(16+12*cursor_position,0,units,1);//выводим 0
			}
	if (!screen_auto_refresh)lcd8544_dma_refresh();

}



void lcd_time_update(void){


#define symbolWidth 5
#define	symbolHeight 8
#define x_pos 59
	//16
	//(84-5*symbolWidth+1)
#define y_pos 30
#define  shift 10
	//41
	//(16+5)

	uint8_t tens,units;
//	fillRect(x_pos,y_pos,5*symbolWidth-1,symbolHeight,0);
	fillRect(x_pos,y_pos-shift, 5*symbolWidth-1,symbolHeight*2,0);
	tens = date.hour/10;
	units = date.hour - tens*10;

	drawSmallDigit(x_pos,y_pos,tens,1);
	drawSmallDigit(x_pos+symbolWidth,y_pos,units,1);
	drawSmallDigit(x_pos+symbolWidth*2,y_pos,11,1);//dot

	tens = date.minute/10;
	units = date.minute - tens*10;

	drawSmallDigit(x_pos+symbolWidth*3,y_pos,tens,1);
	drawSmallDigit(x_pos+symbolWidth*4,y_pos,units,1);

	tens = date.day/10;
	units = date.day - tens*10;
	drawSmallDigit(x_pos,y_pos-shift,tens,1);
	drawSmallDigit(x_pos+symbolWidth,y_pos-shift,units,1);
	drawSmallDigit(x_pos+symbolWidth*2,y_pos-shift,12,1);//dot

	tens = date.month/10;
	units = date.month - tens*10;

	drawSmallDigit(x_pos+symbolWidth*3,y_pos-shift,tens,1);
	drawSmallDigit(x_pos+symbolWidth*4,y_pos-shift,units,1);


//	fillRect(x_pos,y_pos,5*symbolWidth-1,symbolHeight,0);

/*	drawSmallDigit(x_pos+symbolWidth*5,y_pos,11,1);//dot

	tens = date.second/10;
	units = date.second - tens*10;

	drawSmallDigit(x_pos+symbolWidth*6,y_pos,tens,1);
	drawSmallDigit(x_pos+symbolWidth*7,y_pos,units,1);
*/
//	if (!screen_auto_refresh)lcd8544_dma_refresh();

}

#endif
