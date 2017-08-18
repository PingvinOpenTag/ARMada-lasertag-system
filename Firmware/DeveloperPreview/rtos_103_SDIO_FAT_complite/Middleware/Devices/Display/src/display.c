#include "display.h"

void display_clear_screen(void){
#ifndef 	COLOR_LCD
						clear_screen();
						if (!screen_auto_refresh) lcd8544_dma_refresh(); // вывод буфера на экран ! без этого ничего видно не будет !
#else
						ClrScrn();
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


void display_put_string(unsigned char x, unsigned char y, const unsigned char str[]){

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
							 	PStr(/*"Set BT"*/str, &TS);
							 	xSemaphoreGive(xColorLCDSemaphore);
							 }
#endif




}



unsigned char display_standard_symbol_height(void){

		return (unsigned char) STANDARD_SYMBOL_HEIGHT;

}


unsigned char display_standard_symbol_width(void){
		return (unsigned char) STANDARD_SYMBOL_WIDTH;

}


unsigned char display_horizontal_screen_resolution(void){
	return HORIZONTAL_SCREEN_RESOLUTION;
}


unsigned char display_vertical_screen_resolution(void){
	return VERTICAL_SCREEN_RESOLUTION;
}
