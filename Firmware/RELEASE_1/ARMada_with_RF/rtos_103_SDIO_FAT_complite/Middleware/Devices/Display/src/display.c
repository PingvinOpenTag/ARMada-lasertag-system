#include "types.h"
#ifdef DISPLAY_ENABLE
#include "display.h"
#include <stdio.h>
#include <stdint.h>


extern void clear_screen(void);
extern void CounterToFtime(uint32_t counter,ftime_t * ftime);

void display_clear_screen(void){//потокобезопасна
#ifndef 	COLOR_LCD
						clear_screen();
						if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#else
						 if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
						 {
							ClrScrn();
							xSemaphoreGive(xColorLCDSemaphore);
						 }
#endif
}


void display_init(void){
#ifndef 	COLOR_LCD
		lcd8544_init(); // инициализация чёрно-белого дисплея
#else

		ILI9163Init();//
/*
		drawBMP("2gun.bmp");
		vTaskDelay((portTickType)(TIC_FQR*2));
*/

#endif


}




void display_put_string(unsigned char x, unsigned char y, const unsigned char str[]){//потокобезопасна

#ifndef 	COLOR_LCD
						  lcd8544_putstr(x/*/STANDARD_SYMBOL_WIDTH*/, y/*/STANDARD_SYMBOL_HEIGHT*/, str, 0); // вывод первой строки
						  if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#else
							 static volatile TextParamStruct TS;
							 if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
							 {
							 	while (!(SPI3->SR & SPI_SR_TXE)); // Wait for bus free
							 	while (SPI3->SR & SPI_SR_BSY);
							 	init_spi3();
							 	SB(0x36, Reg); //Set Memory access mode
#if    		LCD_MODUL_VERSION == 2
			SB((0x08 |(1<<7)|(1<<6)), Dat);
#elif 	LCD_MODUL_VERSION == 1
		 	SB((0x08 /*|(1<<7)*/), Dat);
#endif
							 	TS.Size = 0;
							 	TS.Font = StdFont;
							 	TS.XPos =  (uint8_t)x;
							 	TS.YPos = (uint8_t)y;
							 	TS.TxtCol = iWhite;//iGreen;//iRed;
							 	TS.BkgCol =iBlack;//iWhite;
							 	PStr(/*"Set BT"*/(const char*)str, (TextParamStruct *)&TS);
							 	xSemaphoreGive(xColorLCDSemaphore);
							 }
#endif




}


void display_show_logo(void){ //не потокобезопасна

#ifndef 	COLOR_LCD
		clear_screen();
		xSemaphoreTake( xKeysSemaphore, 30 );//очищаем симафор
		xSemaphoreTake(xWavPlayerManagerSemaphore, 30);//очищаем симафор
	  lcd8544_rect(1,1,83,40,1); // прямоугольник
	  if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
	  lcd8544_putstr(24, 8, (const unsigned char*)"ARMADA", 0); // вывод первой строки
      lcd8544_putstr( 21, 20, (const unsigned char*)"MADE IN", 0); // вывод второй строки
      lcd8544_putstr( 24, 28,(const unsigned char*) "RUSSIA", 0); // вывод второй строки
	  if (!screen_auto_refresh)lcd8544_dma_refresh();

#else
		drawBMPfromFlashUseDMA((uint16_t*) gerb_pic,0,0,128,128,Rotation_0);
#endif
}


#if DEVICE_ROLE==TAG

void display_show_guns_picture(void){//потокобезопасна
#ifndef 	COLOR_LCD
	  drawBitmap(0,0,(const unsigned char *)black_gun_image,84,48,1);
	  if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
	  if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
	  					 {

	  						drawBMPfromFlashUseDMA((uint16_t*) guns,0,0,128,128,Rotation_0);


	  						xSemaphoreGive(xColorLCDSemaphore);
	  					 }

#endif

}
#endif

void display_show_bluetooth_icon(void){//потокобезопасна
#ifndef 	COLOR_LCD
	  drawDigit(52+2,0,10,1);//рисуем эмблемку блютус
	  if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
		{
		drawBMPfromFlash((uint16_t *)bluetooth1,16,32);
		xSemaphoreGive(xColorLCDSemaphore);
		}
#endif
}

void display_hide_bluetooth_icon(void){//потокобезопасна
#ifndef 	COLOR_LCD
	fillRect(52+2,0,12,16,0);//удаляем эмблемку блютус
	  if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
  {
	 FillRect(iBlack,0,0,16,32);
	 xSemaphoreGive(xColorLCDSemaphore);
   }
#endif
}


void display_update_bluetooth_status(void){//потокобезопасна
	  if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//если соединение установлено
	  {
		  display_show_bluetooth_icon();
	  }
	  else
	  {
		  display_hide_bluetooth_icon();
	  }
}




unsigned char display_standard_symbol_height(void){//потокобезопасна

		return (unsigned char) STANDARD_SYMBOL_HEIGHT;

}


unsigned char display_standard_symbol_width(void){//потокобезопасна
		return (unsigned char) STANDARD_SYMBOL_WIDTH;

}


unsigned char display_horizontal_screen_resolution(void){//потокобезопасна
	return HORIZONTAL_SCREEN_RESOLUTION;
}


unsigned char display_vertical_screen_resolution(void){//потокобезопасна
	return VERTICAL_SCREEN_RESOLUTION;
}

extern const unsigned char heart[4][32];
void display_show_heart_picture(uint8_t picture_index){//потокобезопасна
	if(picture_index>3) return;
#ifndef COLOR_LCD
            fillRect(0,0,16,16,0);
            drawBitmap(0,0,heart[picture_index],16,16,1);
            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
 //           LCD_BL_ON;
            uint16_t* pHeardPic;
            if (picture_index==0) pHeardPic = (uint16_t*)heart_1;
            else if (picture_index==1)pHeardPic = (uint16_t*)heart_2;
            else if (picture_index==2)pHeardPic = (uint16_t*)heart_3;
            else if (picture_index==3)pHeardPic = (uint16_t*)heart_4;

            if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
            {
            drawBMPfromFlashUseDMA(pHeardPic,heart_x_pos,heart_y_pos,55,48,Rotation_0);
           	xSemaphoreGive(xColorLCDSemaphore);
            }
#endif
}



void display_update_time(void){//потокобезопасна
	CounterToFtime ((uint32_t)RTC_GetCounter(),(ftime_t *)&date);
#ifndef COLOR_LCD
            	lcd_time_update();
#else
 //           	if(armadaSystem.wav_player.stop){
            		color_lcd_time_update();
 //           	}

#endif


}




void display_update_health(void){//потокобезопасна
#ifndef COLOR_LCD
	  lcd_health_update();
#else
	  color_lcd_health_update();
#endif
}



void display_update_rounds(void){//потокобезопасна
#ifndef COLOR_LCD
	  lcd_rounds_update();
#else
	  color_lcd_rounds_update();
#endif
}

void display_update_clips(void){//потокобезопасна
#ifndef COLOR_LCD
	lcd_clips_update();
#else
	color_lcd_clips_update();
#endif
}

void display_update_battary_voltage(void){//потокобезопасна

	int ref_adc_val;
	uint32_t voltage;
	ref_adc_val =  ADC_values[3];//ADC_GetConversionValue(ADC1);

	#ifdef LOW_DROP_ENABLE

	            	voltage =100+(2400*(uint16_t)ADC_values[1])/ref_adc_val;//1200mV - опорное напряжение
	#else
	            	voltage = (1200*4096)/ref_adc_val;//1200mV - опорное напряжение
	#endif


#ifndef COLOR_LCD
	if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
	{
		lcd_battary_voltage_update(voltage);
	   	xSemaphoreGive(xGameOverSemaphore);
	}
#else
	color_lcd_battary_voltage_update(voltage);
#endif
}

#ifndef 	COLOR_LCD
extern const char clip_img[7][48];
extern const char battary_img[16];
#endif


void display_init_gui(void){
	display_clear_screen();
#ifndef 	COLOR_LCD
display_backlight_off();
drawBitmap(84-16,0, (const unsigned char*)battary_img,16,8,1);
drawSmallDigit(84-16-1,8-1,0,1);
drawSmallDigit(84-16+4-1,8,10,1);
drawSmallDigit(84-16+8-1,8-1,0,1);
drawSmallDigit(84-16+12,8-1,0,1);
drawBitmap(0,16,(const unsigned char*)&clip_img[6][0],16,24,1);
drawDigit(16,16+4,3,1);
drawDigit(28,16+4,5,1);

drawBitmap(0,0,heart[0],16,16,1);
//drawDigit(40,0,0,1);
drawSmallDigit(0,41,1,1);
drawSmallDigit(5,41,5,1);
#else
if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)

{
	drawBMPfromFlashUseDMA((uint16_t*)bullet,64,40,64,21,Rotation_90);
	drawBMPfromFlashUseDMA((uint16_t*)clip_2,93,40,35,64,Rotation_0);

	xSemaphoreGive(xColorLCDSemaphore);
}


#endif


display_update_health();
display_update_rounds();
display_update_clips();

#ifdef COLOR_LCD
taskENTER_CRITICAL();
#endif

display_update_fire_mode_status();

#ifdef COLOR_LCD
taskEXIT_CRITICAL();
#endif

display_update_bluetooth_status();
}


void display_update_fire_mode_status(void){
#ifndef 	COLOR_LCD
		lcd_fire_mode_update(curr_fire_mode);
#else
		color_lcd_fire_mode_update(curr_fire_mode);

#endif
}


void display_backlight_off(void){
	GPIO_SetBits(GPIOC, GPIO_Pin_13);//выключаем подсветку ЖКИ
}

void display_backlight_on(void){
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);//включаем подсветку ЖКИ
}


void display_show_game_over_picture(void){
#ifndef 	COLOR_LCD
														clear_screen();
														drawBitmap(0,0,(const unsigned char*)game_over_black_image,84,48,1);
											            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
													if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если LCD занят, ждем 2 с
													 {
														drawBMPfromFlashUseDMA((uint16_t*)game_over_pic,0,0,128,128,Rotation_0);

														xSemaphoreGive(xColorLCDSemaphore);
													 }
#endif
}



#endif
