/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "fatfs.h"
#include "usb_device.h"
#include "types.h"

/* USER CODE BEGIN Includes */







#define SOUND_BUFFER_SIZE 512


//#define ApplicationAddress 0x8014000 //Это адрес куда будет переходить
#define ApplicationAddress 0x8000000 //Это адрес куда будет переходить
#define BUFF_PAGE_ADDRESS  0x807F800 //последняя страница для хранения стек поинтера и reset handle
#define MASS_STORAGE_ADDRESS 0x8073800 //адрес, по которому размещён mass storage

//#define USB_ATTACH_PIN GPIO_PIN_4
#define USB_ATTACH_PIN GPIO_PIN_6
//GPIO_PIN_6
//GPIO_PIN_4
//#define USB_ATTACH_PORT GPIOB
#define USB_ATTACH_PORT GPIOC
//GPIOC
//GPIOB
#define USB_ATTACH


typedef void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

typedef enum buffer_state {
	Busy,
	HT_done,
	TC_done
} tbuff_state;




typedef void
(* const pHandler)(void);

extern unsigned int _estack;
extern unsigned int _isr_real;

#define FLASH_BEGIN               0x8000000

uint32_t firstPageAddr = 0x8000000;
uint32_t secondPageAddr = 0x8000000 + FLASH_PAGE_SIZE;

uint32_t lastPageAddr = ((uint32_t) &_isr_real) - FLASH_PAGE_SIZE;

uint8_t firstPage[FLASH_PAGE_SIZE];
uint32_t fistPageLen = 0;

uint8_t buffer[FLASH_PAGE_SIZE];
uint32_t bufferLen = 0;


void Reset_Handler();

__attribute__ ((section(".isr_vector_stub"),used))
pHandler isr_vectors_stub[2] =
{
	// Core Level - CM3
	(pHandler) &_estack, // The initial stack pointer
	Reset_Handler        // The reset handler
	// Other vectors are unnecessary
};



/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;
uint16_t DAC_Buff[SOUND_BUFFER_SIZE];
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

static FATFS fatfs;
static FIL fil;
static FILINFO info;
TIM_HandleTypeDef htim6;
static volatile uint16_t dac_buff_pos = 0;
static volatile uint16_t bitsPerSample;
static volatile tbuff_state  buff_state =Busy;
char string_buffer[512];
DAC_HandleTypeDef hdac;
tmemoryPageStatus pageStatus[256];
const uint8_t sensors[4] = {BLACK,BLACK,BLACK,BLACK} ;
const char flashFileName[] = "firmware.bin";

thexRecordData recData;

uint32_t lba;//Linear Base Address
uint32_t ulba;//Upper Linear Base Address (верхний линейный базовый адрес, ULBA)
/*
* Запись 32-bit Extended Linear Address Record используется
* для указания битов 16..31 от Linear Base Address (линейный базовый адрес, LBA),
* где биты 0..15 адреса LBA равны 0.
* Биты 16..31 LBA называются Upper Linear Base Address (верхний линейный базовый адрес, ULBA).
* Абсолютный адрес памяти в байте содержимого последующей Data Record получается
* путем добавления LBA к смещению, вычисляемому путем добавления поля LOAD OFFSET,
* содержащегося в Data Record, к индексу байта в Data Record (0, 1, 2, ... n).
* Это добавление смещения делается по модулю 4G (32 бита),
* с игнорированием переноса, так что загрузка смещения (от 0FFFFFFFFH до 000000000H) приводит
* к циклическому огибанию от конца к началу 4G линейного адреса, задаваемого LBA.
* Линейный адрес, по которому загружается каждый отдельный байт, при этом вычисляется
* по формуле:
* (LBA + DRLO + DRI) MOD 4G
* Где DRLO равно полю LOAD OFFSET в записи Data Record,
* DRI равен индексу байта в записи Data Record.
* Когда запись Extended Linear Address Record задает значение LBA,
* оно может появиться в любом месте 32-битного шестнадцатеричного объектного файла.
* Это значение остается эффективным, пока не появится
* другая запись Extended Linear Address Record.
* Значение по умолчанию для LBA равно нулю,
* пока не появится запись Extended Linear Address Record.
 */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_SDIO_SD_Init(void);
void deinitCardreader(void);
static void MX_DAC_Init(void);
static void MX_DAC_DeInit(void);
static void DAC_test(void);
static void MX_TIM6_Init(uint32_t);
static void MX_TIM6_DeInit(void);
static void MX_NVIC_Init(void);
static void init_dac_duffer(void);
static char wave_playback(const char*);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
#ifdef USB_ATTACH
static void USB_attach(void);
#endif


//Функция возврщает true когда можно стирать или писать память.
static uint8_t flash_ready(void);
static void flash_erase_page(uint32_t);
static void flash_write(uint32_t,uint32_t);
static bool updateFirmware(void);
void moveVectorTable(uint32_t);
static void startApp(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

extern int isCardreaderActive(void);

/* USER CODE END 0 */
uint32_t getMaxFlashImageSize()
{
	return (uint32_t) &_isr_real - firstPageAddr;
}

HAL_StatusTypeDef erase(uint32_t from, uint32_t to)
{
	HAL_StatusTypeDef res = HAL_OK;
	for (uint32_t i = from; i <= to; i += FLASH_PAGE_SIZE)
	{
		FLASH_EraseInitTypeDef erase;
		erase.TypeErase = FLASH_TYPEERASE_PAGES;
		erase.Banks = FLASH_BANK_1;
		erase.PageAddress = i;
		erase.NbPages = 1;
		uint32_t error = 0;
		res = HAL_FLASHEx_Erase(&erase, &error);
		if (res != HAL_OK) {
			printf("Error while erasing page at %lX\n", i);
			return res;
		}
	}
	printf("Pages from %lX to %lX erased\n", from, to);
	return res;
}


FRESULT readNextPage(uint8_t *target, uint32_t *readed)
{
	uint16_t blocksCount = 16;
	uint16_t fileBlock = FLASH_PAGE_SIZE / blocksCount;
	*readed = 0;
	UINT readedNow = 0;
	FRESULT res = FR_OK;
	for (uint16_t i = 0; i<blocksCount; i++)
	{
		res = f_read(&fil, target, fileBlock, &readedNow);
		target += readedNow;
		*readed += readedNow;
		if (res != FR_OK || readedNow != fileBlock)
			break;
	}
	return res;
}


HAL_StatusTypeDef flashWrite(uint32_t position, uint8_t *data, uint32_t size)
{
	HAL_StatusTypeDef res = HAL_OK;
	for (uint32_t i=0; i<size; i+=2)
	{
		res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, position + i, *(uint16_t*)&data[i]);
		if (res != HAL_OK)
		{
			printf("Error! Flash programming failed at %lX: %d\n", position+i, res);
			break;
		}
	}
	return res;
}


bool checkFlashFile(void){
	// Testing flash file existance
	FRESULT res = f_stat(flashFileName, &info);
		if (res != FR_OK)
			return false;
		// Checking file size
		if (info.fsize > getMaxFlashImageSize())
				return false;
		return true;
}

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

	HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  HAL_Delay(1000);

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDIO_SD_Init();


  MX_SPI1_Init();
  MX_SPI2_Init();

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2, &sensors[0], 4, 100);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
   // MX_FATFS_Init();

  MX_USB_DEVICE_Init();


  /* USER CODE BEGIN 2 */
//  HAL_Delay(1000);

#ifdef USB_ATTACH
  USB_attach();
#endif

  HAL_Delay(1000);
  if (0 == isCardreaderActive())
  	{

	  deinitCardreader();
	  MX_NVIC_Init();
//	  init_dac_duffer();
//	  DAC_test();

//	  while(1){};
	  MX_FATFS_Init();
	  FRESULT res = f_mount(&fatfs, "", 1);
	  	if (res != FR_OK)
	  	{


	  		volatile int i;
	  		i++;
	  		startApp();

	  	}
	  	else{


	  		res = f_open( &fil , flashFileName,/* FA_CREATE_NEW*/FA_READ | FA_WRITE);
	  		    if ( res == FR_OK ){
//	  		    	MX_NVIC_Init();
	  		    	wave_playback("boot1.wav");


	  		    	if( checkFlashFile())
	  		    	  {
	  		    	    		wave_playback("boot2.wav");
	  		    	    		if( updateFirmware())
	  		    	    		    		{
	  		    	    		    			//удачно обновились
	  		    	    		    			wave_playback("boot4.wav");
	  		    	    		    			f_close(&fil);
	  		    	    				       /* Переименовываем объект */
	  		    	    		    			res = f_unlink ("done.bin");
	  		    	    				       res = f_rename(flashFileName, "done.bin");
	  		    	    				       if(res == FR_OK)
	  		    	    				     	       {

	  		    	    				     	       }


	  		    	    				       f_mount(&fatfs, "", 0);
	  		    	    				       startApp();

	  		    	    		    		}
	  		    	    		    		else
	  		    	    		    		{
	  		    	    		    			//не удалось обновить
	  		    	    		    			f_close(&fil);
	  		    	    		    			wave_playback("boot5.wav");
	  		    	    		    			f_mount(&fatfs, "", 0);
	  		    	    		    			while(1){};//бесконечный цикл
	  		    	    		    		}
	  		    	   }//[if( checkHexFile())]
	  		    	else
	  		    	   {
	  		    	    		//файл не прошел проверку
	  		    	   		wave_playback("boot3.wav");
	  		    	   }

	  	  			f_close(&fil);
	  	  			f_mount(&fatfs, "", 0);

	  		    }//[ if ( res == FR_OK )]
	  		    f_close(&fil);
  	  			f_mount(&fatfs, "", 0);
	  		    startApp();


	  	}
	  		startApp();


  	}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* SDIO init function */
static void MX_SDIO_SD_Init(void)
{
  HAL_SD_ErrorTypedef sd_init_result;
  int attempts = 3;
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 3;

  do{
  sd_init_result = HAL_SD_Init(&hsd, &SDCardInfo);
  attempts--;
  }while ((attempts>0)&&(sd_init_result !=SD_OK));

  if (sd_init_result == SD_OK) HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B);
 /*
  else{
	  startApp();
  }
  */
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	/* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  //PB1 - подсветка выстрела, закрываем полевик
  /*Configure GPIO pin Output Level */
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

  //PA8 - IR ключ, закрываем полевик
   /*Configure GPIO pin Output Level */
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
/*
   //PC5 - USB Disconnect - открываем полевик (0 на затвор)
   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
*/
   //PC4 - Bluetooth RESET pin
   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);



   /*Configure GPIO pin : PB1 */
   GPIO_InitStruct.Pin = GPIO_PIN_1;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

   /*Configure GPIO pin : PA8 */
   GPIO_InitStruct.Pin = GPIO_PIN_8;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   //PC4 - Bluetooth RESET pin
   GPIO_InitStruct.Pin = GPIO_PIN_4;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
/*
   //PC5 - USB Disconnect
   GPIO_InitStruct.Pin = GPIO_PIN_5;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
*/
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif




void deinitCardreader()
{
	/* Stop the low level driver  */
	USBD_LL_Stop(&hUsbDeviceFS);

	/* Initialize low level driver */
	USBD_LL_DeInit(&hUsbDeviceFS);
}




/* DAC init function */
static void MX_DAC_Init(void)
{

  DAC_ChannelConfTypeDef sConfig;

    /**DAC Initialization
    */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

    /**DAC channel OUT1 config
    */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* DAC DeInit function */
static void MX_DAC_DeInit(void)
{

  if (HAL_DAC_DeInit(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

}



static void DAC_test(void){

	  MX_DAC_Init();
	  if (HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK)
	   {
	     Error_Handler();
	   }
	  MX_TIM6_Init(44100);
	  MX_NVIC_Init();
	  HAL_TIM_Base_Start_IT(&htim6);
	  /* USER CODE BEGIN 2 */




//	  volatile uint8_t i;
//	    while (1)
//	    {

//	  	  if ( HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_8B_R, i++) != HAL_OK)
//	  	     {
//	  	       Error_Handler();
//	  	     }
//	  	 if (i==255) i=0;

	  	  /* USER CODE END WHILE */

	    /* USER CODE BEGIN 3 */

//	    }



}



/* TIM6 init function */
static void MX_TIM6_Init(uint32_t sampleRate)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period =  (uint16_t)(SystemCoreClock/sampleRate);
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}


/* TIM6 DeInit function */
static void MX_TIM6_DeInit(void)
{


  if (HAL_TIM_Base_DeInit(&htim6) != HAL_OK)
  {
    Error_Handler();
  }

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){


	if (dac_buff_pos == SOUND_BUFFER_SIZE/2-1){
		buff_state = HT_done;
	} else if (dac_buff_pos < SOUND_BUFFER_SIZE-1){

	}
	else {

		dac_buff_pos = 0;
		buff_state = TC_done;
	}

	if ( HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_L , (uint32_t) DAC_Buff[dac_buff_pos++]) != HAL_OK)
			  	     {
			  	       Error_Handler();
			  	     }
}



static void MX_NVIC_Init(void)
{
  /* TIM6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM6_IRQn);
}



static void init_dac_duffer(void){
	for (int i=0; i<SOUND_BUFFER_SIZE; i++){
		DAC_Buff[i]=i;


	}


}

//*********************************************************************************************
//function воспроизвести wave-файл                                                           //
//argument имя файла                                                                         //
//result   0 - все нормально, иначе - ошибка                                                 //
//*********************************************************************************************
static char wave_playback(const char *FileName)
{
	//if (HAL_TIM_Base_DeInit(&htim6) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }
	FIL file;    //файловый объект
	FRESULT res;
	UINT cnt;                                   //количество реально прочитанных байт
	uint32_t sampleRate;
	volatile uint32_t subchunk2Size;//количество байт данных
	res = f_open( &file, FileName, FA_READ );   //открыть файл FileName для чтения
	if(res) return 1;
	res = f_lseek(&file,24);  //переместить указатель на начало полезных данных
	if(res) return 2;
	f_read (&file,(uint32_t*)&sampleRate,4,&cnt);
	if (cnt<4) return 3;
	res = f_lseek(&file,34);
	if(res) return 2;
	f_read (&file,(uint16_t*)&bitsPerSample,2,&cnt);
	if (bitsPerSample != 16) return 4;
	MX_DAC_Init();
	if (HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK)
		   {
		     Error_Handler();
		   }
	MX_TIM6_Init(sampleRate);
	res = f_lseek(&file,40);
	if(res) return 2;
	f_read (&file,(uint32_t*)&subchunk2Size,4,&cnt);
	res = f_lseek(&file,0x2c);
	if(res) return 2;
	f_read (&file,&DAC_Buff[0],SOUND_BUFFER_SIZE*2,&cnt);       //загрузить буфер ЦАПа данными
	if(res) return 3;
	if(subchunk2Size>cnt) subchunk2Size-=cnt;
	else subchunk2Size = 0;
	for(uint16_t i=0;i < SOUND_BUFFER_SIZE; i++)
	  {
			   DAC_Buff[i] +=(uint16_t)0x8000;
			//   DAC_Buff[i] = DAC_Buff[i]>>4;
	  }
	buff_state = Busy;
	HAL_TIM_Base_Start_IT(&htim6);

	while(1)
	{
		while (	buff_state != HT_done){};
		f_read (&file,&DAC_Buff[0],SOUND_BUFFER_SIZE/*256*/,&cnt);    //загрузить ее данными
		if(cnt<SOUND_BUFFER_SIZE)break;                        //если конец файла
		for(uint16_t i=0;i < SOUND_BUFFER_SIZE/2; i++)
		{
		 	 DAC_Buff[i] +=(uint16_t)0x8000;
		}
		while (	buff_state != TC_done){};
		f_read (&file,&DAC_Buff[SOUND_BUFFER_SIZE/2],SOUND_BUFFER_SIZE,&cnt);  //загрузить ее данными
		if(cnt<SOUND_BUFFER_SIZE)break;                        //если конец файла
		for(uint16_t i=0;i < SOUND_BUFFER_SIZE/2; i++)
		{
		 	 DAC_Buff[SOUND_BUFFER_SIZE/2+i] +=(uint16_t)0x8000;
		}


	}
	HAL_TIM_Base_Stop_IT(&htim6);
	f_close(&file);

}






//Функция возврщает true когда можно стирать или писать память.
static uint8_t flash_ready(void) {
return !(FLASH->SR & FLASH_SR_BSY);
}




//Функция стирает одну страницу. В качестве адреса можно использовать любой
//принадлежащий диапазону адресов той странице которую нужно очистить.
static void flash_erase_page(uint32_t address) {
FLASH->CR|= FLASH_CR_PER; //Устанавливаем бит стирания одной страницы
FLASH->AR = address; // Задаем её адрес
FLASH->CR|= FLASH_CR_STRT; // Запускаем стирание
while(!flash_ready()){};  //Ждем пока страница сотрется.
FLASH->CR&= ~FLASH_CR_PER; //Сбрасываем бит обратно
}

static void flash_write(uint32_t address,uint32_t data) {
FLASH->CR |= FLASH_CR_PG; //Разрешаем программирование флеша
while(!flash_ready());{} //Ожидаем готовности флеша к записи
*(__IO uint16_t*)address = (uint16_t)data; //Пишем младшие 2 бата
while(!flash_ready()){};
address+=2;
data>>=16;
*(__IO uint16_t*)address = (uint16_t)data; //Пишем старшие 2 байта
while(!flash_ready()){};
FLASH->CR &= ~(FLASH_CR_PG); //Запрещаем программирование флеша
}









static bool updateFirmware(void){

	uint32_t position = FLASH_BEGIN + FLASH_PAGE_SIZE;
	HAL_StatusTypeDef hal_res = HAL_FLASH_Unlock();
	if (hal_res != HAL_OK)
		{
			return false;
		}
	if (HAL_OK != erase(secondPageAddr, info.fsize + firstPageAddr))
	{
		return false;
	}

	if (FR_OK == readNextPage(firstPage, &fistPageLen))
		{
			printf("First page of flash readed successfuly\n");
			// Reading original SP and Reset_Handler
			flash_erase_page(BUFF_PAGE_ADDRESS);
			flash_write((uint32_t)BUFF_PAGE_ADDRESS, *(uint32_t*)&firstPage[0]);
//			state.mainProgramStackPointer = *(uint32_t*)&firstPage[0];
			flash_write((uint32_t)(BUFF_PAGE_ADDRESS+4), *(uint32_t*)&firstPage[4]);
//			state.mainProgramResetHandler = *(uint32_t*)&firstPage[4];
			// Changing it to bootloader ones

			*(uint32_t*)&firstPage[0] = (uint32_t) &_estack;
			*(uint32_t*)&firstPage[4] = (uint32_t) Reset_Handler;
		} else {
			return false;
		}

	do {
		readNextPage(buffer, &bufferLen);
		if (HAL_OK != flashWrite(position, buffer, bufferLen))
		{
			return false;
		}
		position += bufferLen;
	} while (bufferLen != 0);

	if (HAL_OK != erase(FLASH_BEGIN, FLASH_BEGIN))
	{
		return false;
	}

	if (HAL_OK != flashWrite(FLASH_BEGIN, firstPage, fistPageLen))
		{
			return false;
		}
	HAL_FLASH_Lock();
	return true;
}



void moveVectorTable(uint32_t Offset)
{
	SCB->VTOR = FLASH_BASE | Offset;
}




/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}



/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }

}
#ifdef USB_ATTACH
static void USB_attach(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	 HAL_GPIO_WritePin(USB_ATTACH_PORT, USB_ATTACH_PIN, GPIO_PIN_SET);
/*
	   GPIO_InitStruct.Pin = GPIO_PIN_6;
	   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	   HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
*/


   //USB Disconnect
      GPIO_InitStruct.Pin = USB_ATTACH_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(USB_ATTACH_PORT, &GPIO_InitStruct);

}
#endif
static void startApp(void){

	Callable pReset_Handler = (Callable) (*(__IO uint32_t*) (BUFF_PAGE_ADDRESS + 4));
//	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_SPI_MspDeInit(&hspi1);
	HAL_SPI_MspDeInit(&hspi2);
	MX_DAC_DeInit();

//	MX_TIM6_DeInit();
	HAL_DeInit();
	   	SysTick->CTRL = 0;// Запретил работу SysTick и его прерывания
//	  		    	    				    NVIC_SetVectorTable(NVIC_VectTab_FLASH, ApplicationAddress);
	  moveVectorTable(0x00);
//	   JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
//	   Jump_To_Application = (pFunction) JumpAddress;
//**//	   __set_MSP (*(__IO uint32_t*)ApplicationAddress);// устанавливаем указатель стека
	   __set_MSP (*(__IO uint32_t*)BUFF_PAGE_ADDRESS);// устанавливаем указатель стека


//	   Jump_To_Application ( ); //Здесь собственно переход по адресу заданному в глобальных переменных
		pReset_Handler();

}






/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
