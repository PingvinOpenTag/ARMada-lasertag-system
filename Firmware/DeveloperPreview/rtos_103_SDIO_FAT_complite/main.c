//http://we.easyelectronics.ru/STM32/programmnyy-dekoder-mp3-na-stm32f10x-chast-2-zapusk-cap.html
//http://we.easyelectronics.ru/STM32/programmnyy-dekoder-mp3-na-stm32f10x-chast-2-zapusk-cap.html
// ���������� ���������� CMSIS

#include "armada.h"
#include "commands.h"

#include "f10x-pcd8544.h" // �������� ���� ���� ��� ��������� ������� !
//#include "math.h"


//const uint32_t key_A [4] = {0x00112233,0x44556677,0x8899aabb,0xccddeeff};
//const uint32_t key_B [4] = {0xffeeddcc,0xbbaa9988,0x77665544,0x33221100};


//���������� �����
extern volatile unsigned char usart1_TxBuf[SIZE_BUF];
extern volatile unsigned char usart1_txBufTail;
extern volatile unsigned char usart1_txBufHead;
extern volatile unsigned char usart1_txCount;

//�������� �����
extern volatile unsigned char usart1_RxBuf[SIZE_BUF];
extern volatile unsigned char usart1_rxBufTail;
extern volatile unsigned char usart1_rxBufHead;
extern volatile unsigned char usart1_rxCount;



/*******************************************************************/
// ������� ������� � �������� ������ � ���������� ��� �������� ������
// � ����� �����
uint32_t writeBuffer[16];
char readBuffer[256];
SD_CardInfo SDCardInfo;

static const char battary_img[16] =
{ 0x00, 0x0C, 0x3F, 0x21, 0x21, 0x21, 0x21, 0x21,
  0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x3F};


static const char image_1[32] =
{ 0x40, 0x40, 0xF0, 0x4C, 0x44, 0x46, 0x42, 0xFF,
  0x41, 0x41, 0x41, 0x41, 0x42, 0xC6, 0xF8, 0x40,
  0x00, 0x00, 0x0F, 0x18, 0x30, 0x60, 0x40, 0xFF,
  0x40, 0x40, 0x60, 0x20, 0x30, 0x1F, 0x00, 0x00};

static const char airplane_image[32] =
{ 0x00, 0x20, 0xE0, 0xC0, 0x80, 0x88, 0x98, 0xF0,
  0xE0, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x01, 0x11, 0x19, 0x0F,
  0x07, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00};

static const char symbol_192_12x16[24] =
{ 0x00, 0xF0, 0xF8, 0x1C, 0x0E, 0x06, 0x06, 0x06, 0xFE, 0xFE, 0x02, 0x00,
  0x20, 0x3F, 0x3F, 0x23, 0x03, 0x03, 0x03, 0x23, 0x3F, 0x3F, 0x20, 0x00};

extern const unsigned char heart[4][32];

extern const char clip_img[7][48];
//extern static const char  battary_img[16];
SD_Error Status = SD_OK;

FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc;         /* file objects */
FRESULT res;
UINT br;
SD_Error read_sektor_res;
//volatile unsigned char DAC_Buff[SOUND_BUFFER_SIZE];
uint16_t DAC_Buff[SOUND_BUFFER_SIZE];
//uint16_t Sinus[360];
DMA_InitTypeDef  DMA_InitStructure;
DMA_InitTypeDef  DMA_to_SPI_InitStructure;

//������ ��� ��������� ��������� �������� �����. �������� ���� = damage_value[Damage_xx]
const uint8_t damage_value [] =
{
	1,   // 0000 = 1   (Damage_1)
	2,   // 0001 = 2   (Damage_2)
	4,   // 0010 = 4   (Damage_4)
	5,   // 0011 = 5   (Damage_5)
	7,   // 0100 = 7   (Damage_7)
	10,  // 0101 = 10  (Damage_10)
	15,  // 0110 = 15  (Damage_15)
	17,  // 0111 = 17  (Damage_17)
	20,  // 1000 = 20  (Damage_20)
	25,  // 1001 = 25  (Damage_25)
	30,  // 1010 = 30  (Damage_30)
	35,  // 1011 = 35  (Damage_35)
	40,  // 1100 = 40  (Damage_40)
	50,  // 1101 = 50  (Damage_50)
	75,  // 1110 = 75  (Damage_75)
	100, // 1111 = 100 (Damage_100)
};



int main(void)
{

	xEventQueue = xQueueCreate( 10, sizeof(tsystem_event_type));
	//int iii;
#ifdef RTC_Enable
	BKPinit();
	RTC_Init();
#endif

	init_var();
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef exti;
	NVIC_InitTypeDef nvic;

	// ��������� ������������ GPIO ����� F
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// ������������� ���� PF6, PF7, PF8, PF9 ��� ��������
	// ������������ ����� PF6 � PF7, �� �� ����������������� ��� ������ LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;// | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |  GPIO_Pin_13;//
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_7);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);//��������� ��������� ���
	//��������� ��������
	// ��������� ������������ GPIO ����� B
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);

	GPIO_ResetBits(GPIOC, GPIO_Pin_4);//"���������" ����� RESET ������ ������ � �����

	/* Enable AFIO clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	  /* Connect EXTI7 Line to PC.07 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);
	/* Configure EXTI7 line */
	exti.EXTI_Line = EXTI_Line7;
	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling/*EXTI_Trigger_Rising*/;
	exti.EXTI_LineCmd = ENABLE;

	EXTI_Init(&exti);

	/* Enable and set EXTI9_5 Interrupt to the lowest priority */
	nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 14;
	nvic.NVIC_IRQChannelSubPriority = 14;
	nvic.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&nvic);





	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	        // ������������ �����
	            Status = SD_Init();
	        // �������� ���������� � �����
	            SD_GetCardInfo(&SDCardInfo);
	                // ����� ����� � ��������� ������ ������
	                SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));
	           //     SD_SetDeviceMode(SD_POLLING_MODE);
	//                SD_SetDeviceMode(SD_INTERRUPT_MODE);
	                // � ��� ������� �� ������ � ������

	                get_uid(request_text);
//	                xtea2_encipher(64,request_text,key_A);
//	                xtea2_decipher(64,request_text,key_A);

	                f_mount(0,&fs);

	init_pwm();
	init_pwm_TIM3();
	init_pwm_TIM2();
	init_pwm_TIM4();
	init_pwm_TIM5();
	DMA_for_ADC_Init();
	init_adc_for_multiscan();


	//	init_adc();
	curr_fire_mode = test_fire_mode_switch();
	last_fire_mode = curr_fire_mode;




	init_TIM7();
	InitCOM1(BAUDRATE);
//	InitCOM2(BAUDRATE);

	init_spi2();
	init_dma_to_spi2();
	init_dac();


//	startSPI2();

//	init_dma();
//	init_timer();
//	TIM_Cmd(TIM6, ENABLE);
//	wave_playback("murakami.wav");
//size_t free;
//	free=xPortGetFreeHeapSize();

	/* ����� �������������� ������� ������ ���� ���� ������. � ���� �������
	     ��������� �������� �������. */
#ifdef COLOR_LCD
	vSemaphoreCreateBinary( xColorLCD_DMA_Semaphore);
	vSemaphoreCreateBinary(xColorLCDSemaphore);//������� �������� LCD
#endif


#ifdef DIRECTION_CALCULATION
	vSemaphoreCreateBinary( xHitDirectionSemaphore);
#endif

	vSemaphoreCreateBinary( xWavPlayerSemaphore);
	vSemaphoreCreateBinary(xWavPlayerManagerSemaphore);
	vSemaphoreCreateBinary(xPlayWavSoundSemaphore);
	vSemaphoreCreateBinary(xHitSemaphore);
	vSemaphoreCreateBinary( xKeysSemaphore );//������� ��� ������, ������������ �������
	vSemaphoreCreateBinary(xSoundBuffHTSemaphore);//������� ���������, ��� �������� ��������� ������ ���������, ���� � �������� �������
	vSemaphoreCreateBinary(xSoundBuffTCSemaphore);//������� ���������, ��� ���� �������� ����� ������, ���� �������� ������ �� ������ �������� ��������� ������

	vSemaphoreCreateBinary(xZone4Semaphore);//������� 4-�� ���� ���������
	vSemaphoreCreateBinary(xZone3Semaphore);//������� 3-�� ���� ���������
	vSemaphoreCreateBinary(xZone2Semaphore);//������� 2-�� ���� ���������
	vSemaphoreCreateBinary(xZone1Semaphore);//������� 1-�� ���� ���������

	vSemaphoreCreateBinary(xUsart1Semaphore);//������� UASRT1
	vSemaphoreCreateBinary(xBluetoothSemaphore);//������� Blutooth
	vSemaphoreCreateBinary(xGameOverSemaphore);//������� Blutooth
	vSemaphoreCreateBinary(xSDcardLockSemaphore);//������� ���������, ��� � SD ������ �������� �����-�� ����

/*
	xTaskCreate( vTaskLED1, ( signed char * ) "LED1", configMINIMAL_STACK_SIZE/2, NULL, 0,
	                        ( xTaskHandle * ) NULL);


	xTaskCreate( vTaskLED2, ( signed char * ) "LED2", configMINIMAL_STACK_SIZE/2, NULL, 1,
	                        ( xTaskHandle * ) NULL);
*/

#ifdef DIRECTION_CALCULATION

#endif

	xTaskCreate( BluetoothTask, ( signed char * ) "BluetoothTask", configMINIMAL_STACK_SIZE, NULL, 1,
					                        ( xTaskHandle * ) NULL);

	xTaskCreate( Wav_Player, ( signed char * ) "Wav_Player", configMINIMAL_STACK_SIZE, NULL, 3,
	                        ( xTaskHandle * ) NULL);


	xTaskCreate( Wav_Player_Manager, ( signed char * ) "Wav_Player_Manager", configMINIMAL_STACK_SIZE/2, NULL, 0,
			                        ( xTaskHandle * ) NULL);
	xTaskCreate( Zone4task, ( signed char * ) "Zone4task", configMINIMAL_STACK_SIZE/2, NULL, 0,
			                        ( xTaskHandle * ) NULL);

	xTaskCreate( Zone3task, ( signed char * ) "Zone3task", configMINIMAL_STACK_SIZE/2, NULL, 0,
				                        ( xTaskHandle * ) NULL);


	xTaskCreate( Zone2task, ( signed char * ) "Zone2task", configMINIMAL_STACK_SIZE/2, NULL, 0,
				                        ( xTaskHandle * ) NULL);


	xTaskCreate( Zone1task, ( signed char * ) "Zone1task", configMINIMAL_STACK_SIZE/2, NULL, 0,
				                        ( xTaskHandle * ) NULL);

#ifdef DIRECTION_CALCULATION
	xTaskCreate( DirectionCalculanionTask, ( signed char * ) "DirectionTask", configMINIMAL_STACK_SIZE/2, NULL, 0,
				                        ( xTaskHandle * ) NULL);
#endif



	vTaskStartScheduler();

//	free=xPortGetFreeHeapSize();
	        while(1)
    {
    }
}


const uint8_t index_to_color[4]={RED,BLUE,YELLOW,GREEN};

volatile uint32_t MSec = 0;

#ifdef DIRECTION_CALCULATION

void DirectionCalculanionTask(void *pvParameters){

	for (;;) {
		xSemaphoreTake(xHitDirectionSemaphore, portMAX_DELAY );

		switch (hit_mask){
		case ((1<<zone_1)|(1<<zone_2)|(1<<zone_3)):
		{

#ifdef SENSORS_BECKLIGHT
			set_sensor_color(RED|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_2);
			set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_2);
#else
			sensors[zone_2]=WHITE|VIBRO_HP;
#endif

		};
		break;
		case ((1<<zone_2)|(1<<zone_3)|(1<<zone_4)):
		{
#ifdef SENSORS_BECKLIGHT
			set_sensor_color(RED|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_3);
			set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_3);
#else
			sensors[zone_3]=WHITE|VIBRO_HP;
#endif




		};
		break;
		case ((1<<zone_3)|(1<<zone_4)|(1<<zone_1)):
		{
#ifdef SENSORS_BECKLIGHT
			set_sensor_color(RED|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_4);
			set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_4);
#else
			sensors[zone_4]=WHITE|VIBRO_HP;
#endif



		};
		break;
		case ((1<<zone_4)|(1<<zone_1)|(1<<zone_2)):
		{
#ifdef SENSORS_BECKLIGHT
			set_sensor_color(RED|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_1);
			set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_1);
#else
			sensors[zone_1]=WHITE|VIBRO_HP;
#endif


		};
		break;
		default:{


		};
		}
		hit_mask =0;
	}

}

#endif



void BluetoothTask(void *pvParameters) {

//	bt_reset();
/*
#ifndef 	COLOR_LCD
		lcd8544_init(); // ������������� �����-������ �������
#else

		ILI9163Init();//

#endif
*/
display_init();// ������������� �������


#ifdef 	COLOR_LCD
	 static volatile TextParamStruct TS;
#endif

	 if (!read_parameters_from_sd_card())
				{
					if (get_settings_from_ini_file())
					{
						save_parameters_to_sd_card();
						set_player_id(armadaSystem.player.id);
						set_team_color(armadaSystem.player.team_color);
						set_gun_damage(armadaSystem.gun.damage);


						#ifdef RTC_Enable
						if(armadaSystem.autostart_game)
						{
							armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
							armadaSystem.gun.rounds = 0;
						}
						else
						{
							armadaSystem.gun.clips = BKP_ReadBackupRegister(BKP_DR2);//armadaSystem.gun.clips_after_start;
							armadaSystem.gun.rounds = BKP_ReadBackupRegister(BKP_DR1);//0;
						}


						#else
							armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
							armadaSystem.gun.rounds = 0;
						#endif

						  configure_bluetooth();


					}
				}
				else {//��������� ������� ��������� �� ��������� �����
						armadaSystem.wav_player.type_of_reproduced_sound=NOTHING;
						armadaSystem.wav_player.type_of_sound_to_play=NOTHING;
						set_player_id(armadaSystem.player.id);
						set_team_color(armadaSystem.player.team_color);
						set_gun_damage(armadaSystem.gun.damage);



#ifdef RTC_Enable
						if(armadaSystem.autostart_game)
						{
							armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
							armadaSystem.gun.rounds = 0;
						}
						else
						{
							armadaSystem.gun.clips = BKP_ReadBackupRegister(BKP_DR2);//armadaSystem.gun.clips_after_start;
							armadaSystem.gun.rounds = BKP_ReadBackupRegister(BKP_DR1);//0;
						}


#else
	#if DEVICE_ROLE==TAG

	#elif DEVICE_ROLE==BANDANA


	#else
						armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
						armadaSystem.gun.rounds = 0;
	#endif
#endif




					}

//	configure_bluetooth();

#ifndef 	COLOR_LCD
		clear_screen();
		xSemaphoreTake( xKeysSemaphore, 30 );//������� �������
		xSemaphoreTake(xWavPlayerManagerSemaphore, 30);//������� �������
	  lcd8544_rect(1,1,83,40,1); // �������������
	  if (!screen_auto_refresh) lcd8544_dma_refresh(); // ����� ������ �� ����� ! ��� ����� ������ ����� �� ����� !
	  LCD_BL_ON;

	  lcd8544_putstr(24, 8, "ARMADA", 0); // ����� ������ ������
      lcd8544_putstr( 21, 20, "MADE IN", 0); // ����� ������ ������
      lcd8544_putstr( 24, 28, "RUSSIA", 0); // ����� ������ ������
	  if (!screen_auto_refresh)lcd8544_dma_refresh();

#else
		drawBMPfromFlashUseDMA(gerb_pic,0,0,128,128,Rotation_0);


	/*
			TS.Size = 1;
			TS.Font = StdFont;
			TS.XPos = 4;
			TS.YPos = 24+6;
			TS.TxtCol = iGreen;
			TS.BkgCol =iBlack;//iWhite;

			PStr("open proj", &TS);
			TS.XPos = 4;
			TS.YPos = 24*2+16;
			TS.TxtCol = iBlue;
			PStr("developed", &TS);
			TS.YPos = 24*3+16;
			TS.XPos = 4+30;
			PStr("in", &TS);
			TS.YPos = 24*4+16;
			TS.XPos = 4+30;


			PStr("RUSSIA", &TS);
			*/
#endif


//////#ifdef RTC_Enable
	  if(xSemaphoreTake( xKeysSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//�������� ������� ����������
//		volatile tsystem_event_type sys_event_tmp;
//		if(xQueueReceive( xEventQueue, &sys_event_tmp, (portTickType)(TIC_FQR*2))== pdTRUE);
		{
		  if (keyboard_event ==key_pressing)
			 {
#ifndef 	COLOR_LCD
			  LCD_BL_OFF;
#endif
			  show_main_menu();
			    //    init_item_list();

			    //getDateClockSettings();
			 }
	  }
//////#else

//	  wave_playback("/syssnd/madeinru.wav");


//////#endif


#ifndef 	COLOR_LCD
	  vTaskDelay((portTickType)(TIC_FQR*2)/*600*/);
	  LCD_BL_OFF;
	  clear_screen();
#if DEVICE_ROLE == TAG
	  drawBitmap(0,0,black_gun_image,84,48,1);
	  if (!screen_auto_refresh)lcd8544_dma_refresh();
#endif
#else
	  vTaskDelay((portTickType)(TIC_FQR*2)/*600*/);

#endif

#if DEVICE_ROLE == TAG
		if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)
					{

#ifndef 	COLOR_LCD

#else
if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
					 {

						drawBMPfromFlashUseDMA(guns,0,0,128,128,Rotation_0);


						xSemaphoreGive(xColorLCDSemaphore);
					 }

#endif





					}


#else

	  initGUI();
#endif

	  bt_configured=true;
/*
	if (!read_parameters_from_sd_card())
	{
		if (get_settings_from_ini_file())
		{
			save_parameters_to_sd_card();
			set_player_id(armadaSystem.player.id);
			set_team_color(armadaSystem.player.team_color);
			set_gun_damage(armadaSystem.gun.damage);
//			configure_bluetooth();
		}
	}
	else {//��������� ������� ��������� �� ��������� �����
			set_player_id(armadaSystem.player.id);
			set_team_color(armadaSystem.player.team_color);
			set_gun_damage(armadaSystem.gun.damage);
//			configure_bluetooth();
		}

*/

	/*
	sensors[zone_1]=RED;
		sensors[zone_2]=BLUE;
		sensors[zone_3]=GREEN;
		sensors[zone_4]=YELLOW;
	*/

#ifdef SENSORS_BECKLIGHT
	  set_sensor_color(index_to_color[armadaSystem.player.team_color]|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_1);
	  set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_1);
	  set_sensor_color(index_to_color[armadaSystem.player.team_color]|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_2);
	  set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_2);
	  set_sensor_color(index_to_color[armadaSystem.player.team_color]|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_3);
	  set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_3);
	  set_sensor_color(index_to_color[armadaSystem.player.team_color]|HIGH_BRIDHTNESS, NUMBER_OF_SENSORS_FRAMES, zone_4);
	  set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_4);
#else
	sensors[zone_1]=index_to_color[armadaSystem.player.team_color]|VIBRO_HP;
	sensors[zone_2]=index_to_color[armadaSystem.player.team_color]|VIBRO_HP;
	sensors[zone_3]=index_to_color[armadaSystem.player.team_color]|VIBRO_HP;
	sensors[zone_4]=index_to_color[armadaSystem.player.team_color]|VIBRO_HP;
#endif


	vTaskDelay(TIC_FQR);
#ifdef SENSORS_BECKLIGHT
	set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_1);
	 set_sensor_vibro(0,zone_1);
	 set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_2);
	 set_sensor_vibro(0,zone_2);
	 set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_3);
	 set_sensor_vibro(0,zone_3);
	 set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_4);
	 set_sensor_vibro(0,zone_4);
#else
	sensors[zone_1]=BLACK;
	sensors[zone_2]=BLACK;
	sensors[zone_3]=BLACK;
	sensors[zone_4]=BLACK;
#endif
	xTaskCreate( vTaskLED2, ( signed char * ) "LED2", configMINIMAL_STACK_SIZE/2, NULL, 1,
		                        ( xTaskHandle * ) NULL);

	xTaskCreate( vTaskLED1, ( signed char * ) "LED1", configMINIMAL_STACK_SIZE, NULL, 0,
		                        ( xTaskHandle * ) NULL);


bt_reset();

	for (;;) {
		xSemaphoreTake(xBluetoothSemaphore, portMAX_DELAY );
		test_bt_data();
		/*
		while (usart1_rxCount){
			char char_tmp;
			char_tmp = USART1_GetChar();
		}
		*/
	}
}



void Zone1task(void *pvParameters) {
	static portTickType xLastWakeTime;
	static trx_packet rx_pack;
	for (;;) {
		  xSemaphoreTake(xZone1Semaphore, portMAX_DELAY );//���� ������� �� ���������

	       switch(Zone1RxEvent)
	       {
	         case RX_COMPLETE:
	       	   {
	       		   hit_processing(zone_1);
	       		   Zone1RxEvent = NOT_EVENT;//���������� �������
	       		   tim4_rec_state = REC_Idle;//��������� ����� �� 1-�� ����
	           }
	           break;
	           case RX_ERROR:
	           {
	           	   Zone1RxEvent = NOT_EVENT;
	             //      		 tim5_rec_state = REC_Idle;
	           }
	           break;
	           case RX_MESSAGE_COMPLETE://������� ���������
	         	{

	         		message_processing(zone_1);
	         		Zone1RxEvent = NOT_EVENT;
	         		tim4_rec_state = REC_Idle;//��������� ����� �� 1-�� ����

	         	}
	         	break;
	           default: break;
	       }
	 }
}

void Zone2task(void *pvParameters) {
	 for (;;) {
		  xSemaphoreTake(xZone2Semaphore, portMAX_DELAY );//���� ������� �� ���������

	       switch(Zone2RxEvent)
	       {
	        case RX_COMPLETE:
	       	   {
	       		/*
	       		   trx_packet rx_pack;
	               rx_pack = get_packet_value(zone_2);
	               sensors[zone_2]=index_to_color[rx_pack.team_id];
	               vTaskDelay(300);
	               sensors[zone_2]=BLACK;
	           */
	       		   hit_processing(zone_2);
	       		   Zone2RxEvent = NOT_EVENT;//���������� �������
	               tim2_rec_state = REC_Idle;//��������� ����� �� 2-�� ����

	           }
	        break;
	        case RX_ERROR:
	           {
	        	   Zone2RxEvent = NOT_EVENT;
	       //      		 tim5_rec_state = REC_Idle;
	           }
	        break;
	        case RX_MESSAGE_COMPLETE://������� ���������
	        {
         		message_processing(zone_2);
	        	Zone2RxEvent = NOT_EVENT;
	            tim2_rec_state = REC_Idle;//��������� ����� �� 2-�� ����
	        }
	        break;
	        default: break;
	       }
	 }
}

void Zone3task(void *pvParameters) {
	 for (;;) {
		  xSemaphoreTake(xZone3Semaphore, portMAX_DELAY );//���� ������� �� ���������
		  switch(Zone3RxEvent)
		        {
		         case RX_COMPLETE:
		        	   {
		                /*
		        		trx_packet rx_pack;
		                rx_pack = get_packet_value(zone_3);
		                sensors[zone_3]=index_to_color[rx_pack.team_id];
		               	vTaskDelay(300);
		               	sensors[zone_3]=BLACK;
		                */
			       		hit_processing(zone_3);
		                Zone3RxEvent = NOT_EVENT;//���������� �������
		                tim3_rec_state = REC_Idle;//��������� ����� �� 3-�� ����
		            }
		         break;
		         case RX_ERROR:
		            {
		         	   Zone3RxEvent = NOT_EVENT;
		 //      		 tim5_rec_state = REC_Idle;
		            }
		         break;
		         case RX_MESSAGE_COMPLETE://������� ���������
		        	{
		        		message_processing(zone_3);
		        		Zone3RxEvent = NOT_EVENT;
		        		tim3_rec_state = REC_Idle;//��������� ����� �� 3-�� ����
		        	}
		         default: break;
		        }
	 }
}

void Zone4task(void *pvParameters) {
//static trx_packet last_proc_pack;
        for (;;) {
       xSemaphoreTake(xZone4Semaphore, portMAX_DELAY );//���� ������� �� ���������
       switch(Zone4RxEvent)
       {
       case RX_COMPLETE:
       	   {
       		  /*
       		  trx_packet rx_pack;
       		  rx_pack = get_packet_value(zone_4);
       		  sensors[zone_4]=index_to_color[rx_pack.team_id];
       		  vTaskDelay(300);
       		  sensors[zone_4]=BLACK;
       		  */
       		  hit_processing(zone_4);
       		  Zone4RxEvent = NOT_EVENT;//���������� �������
       		  tim5_rec_state = REC_Idle;//��������� ����� �� 4-�� ����
       	   }
       	   break;
       case RX_ERROR:
       	   {

       		 Zone4RxEvent = NOT_EVENT;
 //      		 tim5_rec_state = REC_Idle;
       	   }
       	   break;
       case RX_MESSAGE_COMPLETE://������� ���������
      	   {
      		   message_processing(zone_4);
      		   Zone4RxEvent = NOT_EVENT;
      		   tim5_rec_state = REC_Idle;//��������� ����� �� 4-�� ����
      	   }
       default: break;
       }
   }
}




#define heart_x_pos 0
#define heart_y_pos 40
void vTaskLED1(void *pvParameters) {
//	SPISend(~(1<<4));//�����
/*
	int16_t a,b;
	uint16_t c,d;

	float k;
	c=80;
	d=100;
	k = (float)c/(float)d;
	a=-100;
	b = 100;
	a =a *k;
	b =b *k;
	a=a+0x8000;
	b=b+0x8000;
	c = (uint16_t)a;
	d = (uint16_t)b;
*/


	uint16_t r_time=600;

	for (;;) {

		if(!bt_configured)
	   		{
	   			continue;
	   		}
		 // vTaskDelay((portTickType)r_time/*2*/);

	//		bool at_res;
            static volatile int increment = 1;
            static volatile portTickType interval=100;
            vTaskDelay(200);
if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
{

#ifndef COLOR_LCD
            fillRect(0,0,16,16,0);
            drawBitmap(0,0,heart[0],16,16,1);
            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
            LCD_BL_ON;
            if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
            {
            drawBMPfromFlashUseDMA(heart_1,heart_x_pos,heart_y_pos,55,48,Rotation_0);
           	xSemaphoreGive(xColorLCDSemaphore);
            }
#endif
           	xSemaphoreGive(xGameOverSemaphore);
            vTaskDelay(25);
}



if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
{

#ifndef COLOR_LCD
            fillRect(0,0,16,16,0);
            drawBitmap(0,0,heart[1],16,16,1);
            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
//            LCD_BL_ON;
            if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
            {
            drawBMPfromFlashUseDMA(heart_2,heart_x_pos,heart_y_pos,55,48,Rotation_0);
            xSemaphoreGive(xColorLCDSemaphore);
            }
#endif
           	xSemaphoreGive(xGameOverSemaphore);
			vTaskDelay(25);
}




if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
{
#ifndef COLOR_LCD
            fillRect(0,0,16,16,0);
//            clear_screen();
            drawBitmap(0,0,heart[2],16,16,1);
            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
            if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
            {
            drawBMPfromFlashUseDMA(heart_3,heart_x_pos,heart_y_pos,55,48,Rotation_0);
            xSemaphoreGive(xColorLCDSemaphore);
            }
#endif
           	xSemaphoreGive(xGameOverSemaphore);
            vTaskDelay(25);
}





if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
{
#ifndef COLOR_LCD
            fillRect(0,0,16,16,0);
//            clear_screen();
            drawBitmap(0,0,heart[3],16,16,1);
            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
            if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
            {
            drawBMPfromFlashUseDMA(heart_4,heart_x_pos,heart_y_pos,55,48, Rotation_0);
            xSemaphoreGive(xColorLCDSemaphore);
            }
#endif
        	xSemaphoreGive(xGameOverSemaphore);
            vTaskDelay(25);
}




if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
{
#ifndef COLOR_LCD
            fillRect(0,0,16,16,0);
//            clear_screen();
            drawBitmap(0,0,heart[2],16,16,1);
            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
            if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
            {
            drawBMPfromFlashUseDMA(heart_3,heart_x_pos,heart_y_pos,55,48, Rotation_0);
            xSemaphoreGive(xColorLCDSemaphore);
            }
#endif

        	xSemaphoreGive(xGameOverSemaphore);
            vTaskDelay(25);
}

if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
{
#ifndef COLOR_LCD
            fillRect(0,0,16,16,0);
//            clear_screen();
            drawBitmap(0,0,heart[1],16,16,1);
            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
            if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
            {
            drawBMPfromFlashUseDMA(heart_2,heart_x_pos,heart_y_pos,55,48, Rotation_0);
            xSemaphoreGive(xColorLCDSemaphore);
            }
#endif

        	xSemaphoreGive(xGameOverSemaphore);
            vTaskDelay(25);
}

if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
{
#ifndef COLOR_LCD
            fillRect(0,0,16,16,0);
//            clear_screen();
            drawBitmap(0,0,heart[0],16,16,1);
            if (!screen_auto_refresh)lcd8544_dma_refresh();
#else
            if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
            {
            drawBMPfromFlashUseDMA(heart_1,heart_x_pos,heart_y_pos,55,48, Rotation_0);
            xSemaphoreGive(xColorLCDSemaphore);
            }
#endif

        	xSemaphoreGive(xGameOverSemaphore);
            vTaskDelay(25);
}

            int ref_adc_val;
            	uint32_t voltage;

            	ref_adc_val =  ADC_values[3];//ADC_GetConversionValue(ADC1);

#ifdef LOW_DROP_ENABLE

            	voltage =100+(2400*(uint16_t)ADC_values[1])/ref_adc_val;//1200mV - ������� ����������
#else
            	voltage = (1200*4096)/ref_adc_val;//1200mV - ������� ����������
#endif


#ifdef RTC_Enable

            	CounterToFtime ((uint32_t)RTC_GetCounter(),&date);
#ifndef COLOR_LCD
            	lcd_time_update();
#else
 //           	if(armadaSystem.wav_player.stop){
            		color_lcd_time_update();
 //           	}

#endif

#endif
#ifndef COLOR_LCD
            if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(25))== pdTRUE )
            {
            		lcd_battary_voltage_update(voltage);
                	xSemaphoreGive(xGameOverSemaphore);
            }
#else
 //           	if(armadaSystem.wav_player.stop) {
            		color_lcd_battary_voltage_update(voltage);
//            	}
#endif
            		 if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 0){//���� ��� ������ ����������
            			 armadaSystem.wav_player.type_of_sound_to_play = SONAR;//���� ������������� ���� ������
            			 xSemaphoreGive(xWavPlayerManagerSemaphore);
            		 }
            		 else{
            			 if (armadaSystem.wav_player.type_of_sound_to_play == SONAR) armadaSystem.wav_player.type_of_sound_to_play==NOTHING;
            		 }
        }

}

void vTaskLED2(void *pvParameters) {
static volatile tsystem_event_type sys_event_tmp;
for (;;)
{
	  xQueueReceive( xEventQueue, &sys_event_tmp, portMAX_DELAY);
	//   	xSemaphoreTake( xKeysSemaphore, portMAX_DELAY );
   	if(!bt_configured)
   		{
   			keyboard_event = no_key_pressing;
   			reload_key_event = no_key_pressing;
   			continue;
   		}

if (sys_event_tmp.event_source == TRIGGER_KEY){
	switch(keyboard_event){
   	case no_key_pressing:
   	{

   	}
   	break;
   	case key_pressing:
   	{
   		if((countdown_shock_time)||(game_status==OUT_OF_GAME))//���� ����� ���� �� ������
   																				// ��� �������
   																				//���������� �������
   		{
   			keyboard_event = no_key_pressing;
            break;
   		}

   		switch(test_fire_mode_switch())
   		{
   			case SINGLE:
   			{
   				DAC->DHR8R2=armadaSystem.gun.ir_power+armadaSystem.gun.ir_power_offset;//100;

   				//	data_tmp = BKP_ReadBackupRegister(BKP_DR1);
   				//	 BKP_WriteBackupRegister(BKP_DR1, BKP_ReadBackupRegister(BKP_DR1)+1 );

	   			if(armadaSystem.gun.rounds)
	   			{
	   				send_ir_shot_package();
	  				armadaSystem.gun.rounds--;
#ifdef RTC_Enable
	   			    BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif

if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//��������, ��� �� ����� � ����
{
#ifndef 	COLOR_LCD
			lcd_rounds_update();
#else
			color_lcd_rounds_update();
#endif
xSemaphoreGive(xGameOverSemaphore);
}
	   			    armadaSystem.wav_player.type_of_sound_to_play = SHOT;
	   				xSemaphoreGive(xWavPlayerManagerSemaphore);
	   			}
	   			else
	   			{
	  				armadaSystem.wav_player.type_of_sound_to_play = EMPTY_CLIP;
	  				xSemaphoreGive(xWavPlayerManagerSemaphore);
	   			}
   			}
   			break;
   			case SEMI_AUTO:
   			{
  				DAC->DHR8R2=armadaSystem.gun.ir_power+armadaSystem.gun.ir_power_offset;//100;
   				volatile TKEYBOARD_EVENT key_status;
	   			key_status = test_keyboard( ADC_values[0]);


  				for (int i=3;i>0;i--)
 	   			{
  		   			key_status = test_keyboard( ADC_values[0]);
  		   			if(key_status!=key_pressing) break;
  					if(armadaSystem.gun.rounds)
   				   	{


  						send_ir_shot_package();
   				   	    armadaSystem.gun.rounds--;
#ifdef RTC_Enable
   		   			    BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif


if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//��������, ��� �� ����� � ����
{
#ifndef 	COLOR_LCD
	lcd_rounds_update();
#else
	color_lcd_rounds_update();
#endif
	xSemaphoreGive(xGameOverSemaphore);
}
   				  	    armadaSystem.wav_player.type_of_sound_to_play = SHOT;
   				  	    xSemaphoreGive(xWavPlayerManagerSemaphore);
   				   	    if(i>1)vTaskDelay((portTickType)(TIC_FQR*60)/armadaSystem.gun.rate);

   				   	}
 	   				else
   				  	{
   				  		armadaSystem.wav_player.type_of_sound_to_play = EMPTY_CLIP;
   				  	    xSemaphoreGive(xWavPlayerManagerSemaphore);
   				  	    keyboard_event = no_key_pressing;
   				  	    break;
   				   	}
 	   			}//[for]


   			}
   			break;
   			case AUTO:
   			{
   				volatile TKEYBOARD_EVENT key_status;
	   			key_status = test_keyboard( ADC_values[0]);
	   			while (key_status==key_pressing)
	   			{
	   				if(game_status==OUT_OF_GAME)//���� ����� ���� �� ������
	   				   																				// ��� �������
	   				   																				//���������� �������
	   				   		{
	   				   			keyboard_event = no_key_pressing;
	   				            break;
	   				   		}

	   				DAC->DHR8R2=armadaSystem.gun.ir_power+armadaSystem.gun.ir_power_offset;//100;
	   				if(armadaSystem.gun.rounds)
	   				{


	   					send_ir_shot_package();
	   					armadaSystem.gun.rounds--;
#ifdef RTC_Enable
		   			    BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif


if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//��������, ��� �� ����� � ����
{
#ifndef 	COLOR_LCD
	lcd_rounds_update();
#else
	color_lcd_rounds_update();
#endif
	xSemaphoreGive(xGameOverSemaphore);
	}

						armadaSystem.wav_player.type_of_sound_to_play = SHOT;
	   					xSemaphoreGive(xWavPlayerManagerSemaphore);
	   					vTaskDelay((portTickType)(TIC_FQR*60)/armadaSystem.gun.rate);
	   					key_status = test_keyboard(ADC_values[0]);
	   				}
   				   	else
   				   	{
   				   		armadaSystem.wav_player.type_of_sound_to_play = EMPTY_CLIP;
   				   	    xSemaphoreGive(xWavPlayerManagerSemaphore);
   				   	    keyboard_event = no_key_pressing;
   				   	    break;
   				   	}
   				}   //[while]
   			}


   			break;
   			default: break;



   		} //[switch(test_fire_mode_switch())]




   		keyboard_event = no_key_pressing;
   	}	//[case key_pressing]
   	break;
   	default:
   	{
   		keyboard_event = no_key_pressing;
   	}
   	break;
  }//[switch]
}//[if]
else if(sys_event_tmp.event_source == RELOAD_KEY){


 switch(reload_key_event){
  	case no_key_pressing:
   	{

  	}
   	break;
  	case key_pressing:
                   	{
 //                 		GPIO_ResetBits(GPIOC, GPIO_Pin_3); // SS = 0
 //                  		SPISend(0x08);

       //       		vTaskDelay(1);
       //         		GPIO_SetBits(GPIOC, GPIO_Pin_3);
if(game_status==OUT_OF_GAME)//���� ����� ���� �� ������
   												// ��� �������
												//���������� �������
{
	reload_key_event = no_key_pressing;
    break;
}

                   		if(armadaSystem.gun.clips)
                   		{
                   			armadaSystem.gun.rounds = armadaSystem.gun.rounds_placed_in_clip;
#ifdef RTC_Enable
                   			BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif
                   			armadaSystem.gun.clips--;
#ifdef RTC_Enable
                   			BKP_WriteBackupRegister(BKP_DR2, armadaSystem.gun.clips);
#endif

                   			armadaSystem.wav_player.type_of_sound_to_play = RELOAD;

                   			xSemaphoreGive(xWavPlayerManagerSemaphore);
                   			vTaskDelay((portTickType)armadaSystem.gun.reload_time*3);



if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//��������, ��� �� ����� � ����
{
#ifndef 	COLOR_LCD
	lcd_rounds_update();
	lcd_clips_update();
//	lcd_fire_mode_update(curr_fire_mode);
#else
	color_lcd_rounds_update();
	color_lcd_clips_update();
//	color_lcd_fire_mode_update(curr_fire_mode);
#endif
	xSemaphoreGive(xGameOverSemaphore);
}





							keyboard_event = no_key_pressing;//���� �� ����� ����������� ��� ����� ���� - ������� ��� �������
                   		}

                   		reload_key_event = no_key_pressing;
                   	}
                   	break;
                   	default:
                   	{
                   		reload_key_event = no_key_pressing;
                   	}
                   	break;
               }//[switch(reload_key_event)]
		}//[else if(sys_event_tmp.event_source == TRIGGER_KEY)]

else if(sys_event_tmp.event_source == FIRE_MODE_SWITCH){

if(game_status!=OUT_OF_GAME)//���� �������
													//����������
{

if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR/2))==pdTRUE)	//��������, ��� �� ����� � ����
{
#ifndef 	COLOR_LCD
			lcd_fire_mode_update(curr_fire_mode);
#else
			color_lcd_fire_mode_update(curr_fire_mode);
#endif
			xSemaphoreGive(xGameOverSemaphore);
		}

    }//[if(armadaSystem.player.game_status!=OUT_OF_GAME)]

}

else if(sys_event_tmp.event_source == BLUETOOTH){

		    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//���� ���������� �����������
		        {

#if DEVICE_ROLE == BANDANA
				if(bt_connect_hase_done == false)
				{

					bandana_game_session_ID = (uint32_t) xTaskGetTickCount();

					bt_connect_hase_done = true;

				}

		    	bt_tag_init_with_game_status(game_status, NEW_CONNECT);
/*
		    	static volatile bool tag_start_done = false;
		    	if ((!tag_start_done)&&armadaSystem.autostart_game)
		    	{
		    		tir_message tmp_message;
		    		tmp_message.ID = Command;
		    		tmp_message.param = 0x05;
		    		tmp_message.control_byte = Valid_value;
		    		send_message_by_bluetooth(tmp_message);
		    		tag_start_done = true;

		    	}
*/
#endif

	#ifndef COLOR_LCD
if(game_status!=OUT_OF_GAME){
#if DEVICE_ROLE==TAG
#else
	drawDigit(52+2,0,10,1);//������ �������� ������
	 if (!screen_auto_refresh)lcd8544_dma_refresh();
#endif
}
	#else //COLOR_LCD
		    		 if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
		    		{
		    		drawBMPfromFlash(bluetooth1,16,32);
		    		xSemaphoreGive(xColorLCDSemaphore);
		    		}
	#endif
		        }
		    else//���� ���������� ���������
		        {
#ifndef COLOR_LCD

if(game_status!=OUT_OF_GAME){

#if DEVICE_ROLE==TAG
	if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE){
	game_status=OUT_OF_GAME;
	clear_screen();
	drawBitmap(0,0,black_gun_image,84,48,1);
	}
#else
	fillRect(52+2,0,12,16,0);//������� �������� ������
#endif
	 if (!screen_auto_refresh)lcd8544_dma_refresh();
}

	#else//COLOR_LCD
if(game_status!=OUT_OF_GAME){
#if DEVICE_ROLE==TAG
	if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE){
	game_status=OUT_OF_GAME;
	if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
						 {

							drawBMPfromFlashUseDMA(guns,0,0,128,128,Rotation_0);


							xSemaphoreGive(xColorLCDSemaphore);
						 }


	}
#else



#endif
}
	    		 if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
	    		{

		    	FillRect(iBlack,0,0,16,32);



		    	xSemaphoreGive(xColorLCDSemaphore);
	    		}
	#endif

		        }
		   }//[else if(sys_event_tmp.event_source == BLUETOOTH)]
//         j = ADC_GetConversionValue(ADC1); // value from 0 to 4095
       //         j++;
        }//[for (;;)]

}

void Wav_Player_Manager(void *pvParameters){
for (;;)
	{
		xSemaphoreTake(xWavPlayerManagerSemaphore, portMAX_DELAY );//���� ������� ��� wav-������
		switch(armadaSystem.wav_player.type_of_sound_to_play)
				{
					case SHOT:
					{
						stopWavPlayer();
						xSemaphoreGive(xPlayWavSoundSemaphore);
					}
					break;
					case GAME_OVER:
					{
						stopWavPlayer();
						xSemaphoreGive(xPlayWavSoundSemaphore);
					}
					break;
					case NOTHING:
					{
						stopWavPlayer();
					}
					break;
					case HIT:
					{
						if(armadaSystem.wav_player.type_of_reproduced_sound==HIT) break;//���� ���� ������� ��� �������������c�
						else {
							while (!stopWavPlayer()){};
							xSemaphoreGive(xPlayWavSoundSemaphore);
						}
					}
					break;
					case EMPTY_CLIP:
					{
//						if(armadaSystem.wav_player.type_of_reproduced_sound==EMPTY_CLIP) break;//���� ���� "������ ������" ��� �������������c�
//						else {
								stopWavPlayer();
								xSemaphoreGive(xPlayWavSoundSemaphore);
//							}
					}
					break;

					case RELOAD:
					{
//						if(armadaSystem.wav_player.type_of_reproduced_sound==RELOAD) break;//���� ���� "������ ������" ��� �������������c�
//						else {
								stopWavPlayer();
								xSemaphoreGive(xPlayWavSoundSemaphore);
//							}
					}
					break;
					case MESSAGE:
					{
//						if(armadaSystem.wav_player.type_of_reproduced_sound==RELOAD) break;//���� ���� "������ ������" ��� �������������c�
//						else {
								stopWavPlayer();
								xSemaphoreGive(xPlayWavSoundSemaphore);
//							}
					}
					break;

					case START_GAME:
					{
						if(armadaSystem.wav_player.type_of_reproduced_sound==START_GAME) break;//���� ���� "������ ������" ��� �������������c�
						else {
								stopWavPlayer();
								xSemaphoreGive(xPlayWavSoundSemaphore);
							}
					}
					break;

					case SONAR:
					{
						if(armadaSystem.wav_player.type_of_reproduced_sound==SONAR) break;//���� ���� "������ ������" ��� �������������c�
						else {
								stopWavPlayer();
								xSemaphoreGive(xPlayWavSoundSemaphore);
							}
					}
					break;
				}
	}
}


void Wav_Player(void *pvParameters){


//	init_dma();
//	init_dac();
//	init_timer();
////	TIM_Cmd(TIM4, ENABLE);
////	TIM_Cmd(TIM2, ENABLE);

	for (;;) {

		xSemaphoreTake(xPlayWavSoundSemaphore, portMAX_DELAY );//���� ������� ��� wav-������
		if(xSemaphoreTake(xSDcardLockSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� SD ����� ������, ���� 2 �
		{

		switch(armadaSystem.wav_player.type_of_sound_to_play)
						{
							case SHOT:
							{

								armadaSystem.wav_player.type_of_reproduced_sound = SHOT;
				//				wave_playback("/gunsnd/shot.wav");
								wave_playback(armadaSystem.wav_player.shot_sound_file_name);
								armadaSystem.wav_player.type_of_reproduced_sound = NOTHING;
							}
							break;
							case GAME_OVER:
							{
								armadaSystem.wav_player.type_of_reproduced_sound = GAME_OVER;
								wave_playback("/gunsnd/dead.wav");
								wave_playback("/gunsnd/gameover.wav");
								//wave_playback("/music/rekviem.wav");
								wave_playback("voron.wav");
							}
							break;
							case NOTHING:
							{

							}
							break;
							case HIT:
							{
								armadaSystem.wav_player.type_of_reproduced_sound = HIT;
								//wave_playback("/gunsnd/damage.wav");
								wave_playback(armadaSystem.wav_player.hit_sound_file_name);
								armadaSystem.wav_player.type_of_reproduced_sound = NOTHING;
							}
							break;

							case EMPTY_CLIP:
							{
								armadaSystem.wav_player.type_of_reproduced_sound = EMPTY_CLIP;
								wave_playback(armadaSystem.wav_player.empty_clip_sound_file_name);
								armadaSystem.wav_player.type_of_reproduced_sound = NOTHING;
							}
							break;


							case RELOAD:
							{
								armadaSystem.wav_player.type_of_reproduced_sound = RELOAD;
								wave_playback(armadaSystem.wav_player.reload_sound_file_name);
								armadaSystem.wav_player.type_of_reproduced_sound = NOTHING;
								save_parameters_to_sd_card();
							}
							break;
							case MESSAGE:
							{
								armadaSystem.wav_player.type_of_reproduced_sound = MESSAGE;
								wave_playback(armadaSystem.wav_player.message_sound_file_name);
								armadaSystem.wav_player.type_of_reproduced_sound = NOTHING;
								save_parameters_to_sd_card();
							}
							break;
							case START_GAME:
							{
								armadaSystem.wav_player.type_of_reproduced_sound = START_GAME;
								wave_playback(armadaSystem.wav_player.start_game_sound_file_name);
								armadaSystem.wav_player.type_of_reproduced_sound = NOTHING;
							}
							break;
							case SONAR:
							{
								armadaSystem.wav_player.type_of_reproduced_sound = SONAR;
								wave_playback(armadaSystem.wav_player.sonar_sound_file_name);
								armadaSystem.wav_player.type_of_reproduced_sound = NOTHING;
							}
							break;


						}


		//		DMA_DeInit(DMA2_Channel3);
//		init_dma();
//		vTaskDelay(500);
//		DMA2_Channel3->CCR &= (uint16_t)(~DMA_CCR1_EN);//��������� DMA.
//		DMA2_Channel3->CNDTR = 512;//������������� ������ ������������� ������ ������.
//		DMA2_Channel3->CCR |= DMA_CCR1_EN;//�������� DMA
//		wave_playback("murakami.wav");
//		vTaskDelay(500);
/*
		wave_playback("angely.wav");
		vTaskDelay(500);
		wave_playback("RudeBoy1.wav");
		vTaskDelay(500);
//		wave_playback("mur16_4.wav");
//		wave_playback("mur16.wav");
		vTaskDelay(500);
//		wave_playback("benny.wav");
		vTaskDelay(500);
//		wave_playback("sabaton.wav");
//		DMA_DeInit(DMA2_Channel3);
//		init_dma();
		vTaskDelay(500);
		wave_playback("murakami.wav");
		vTaskDelay(500);
//		DMA_DeInit(DMA2_Channel3);
//		init_dma();
		wave_playback("sound.wav");
		vTaskDelay(500);
//		init_dma();
		wave_playback("s33.wav");
		vTaskDelay(500);
//		init_dma();
		wave_playback("g1.wav");
		 vTaskDelay(500);
//		 init_dma();
		wave_playback("g2.wav");
		 vTaskDelay(500);
//		 init_dma();
		 wave_playback("g3.wav");
		 vTaskDelay(500);
//		 init_dma();
		 wave_playback("s3.wav");
		 vTaskDelay(500);
//		  init_dma();
		 wave_playback("s4.wav");
		 vTaskDelay(500);
//		 init_dma();
//		 wave_playback("s5.wav");
//		 vTaskDelay(500);
//		 init_dma();
//		 wave_playback("s6.wav");
		 vTaskDelay(500);
//		 init_dma();
		 wave_playback("a1.wav");
		 vTaskDelay(500);
*/
		xSemaphoreGive(xSDcardLockSemaphore);
		}//[if(xSemaphoreTake(xSDcardLockSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)]

	}//[for(;;)]


}

void init_gpio(void) {
GPIO_InitTypeDef gpio_init;
RCC_APB1PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
gpio_init.GPIO_Mode = GPIO_Mode_AIN;
gpio_init.GPIO_Pin = GPIO_Pin_4;
//gpio_init.GPIO_OType = GPIO_OType_PP;
//gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOA, &gpio_init);
}



void init_dma( uint16_t bitsPerSample) {

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	if(bitsPerSample == 16)  {
		DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&DAC->DHR12L1;
		  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DAC_Buff;
		  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		  DMA_InitStructure.DMA_BufferSize = SOUND_BUFFER_SIZE;
		  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	}
	else
	{
	DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&DAC->DHR8R1;
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&DAC_Buff;//&Sinus;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	  DMA_InitStructure.DMA_BufferSize = SOUND_BUFFER_SIZE*2; //512;360;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	}

	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	/* ��������� ���������� �� ��������� �������� ������ */
	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC , ENABLE);
	DMA_ITConfig(DMA2_Channel3, DMA_IT_HT , ENABLE);

	DMA_Cmd(DMA2_Channel3, ENABLE);
    NVIC_SetPriority(DMA2_Channel3_IRQn, 13);
	NVIC_EnableIRQ(DMA2_Channel3_IRQn);

}

void init_dma_to_spi2(void)//����������� DMA ��� ������ � SPI2 (���������)
{

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_to_SPI_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(SPI2->DR);
	DMA_to_SPI_InitStructure.DMA_MemoryBaseAddr = (uint32_t)sensors;//&Sinus;
	DMA_to_SPI_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_to_SPI_InitStructure.DMA_BufferSize = 4;
	DMA_to_SPI_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_to_SPI_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_to_SPI_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_to_SPI_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA_Mode_Circular;
	DMA_to_SPI_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_to_SPI_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel5, &DMA_to_SPI_InitStructure);
	/* ��������� ���������� �� ��������� �������� ������ */
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC , ENABLE);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_HT , ENABLE);

//	DMA_Cmd(DMA1_Channel5, ENABLE);
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);


}
void startSPI2(void){

	DMA_Cmd(DMA1_Channel5, DISABLE);
#ifdef SENSORS_BECKLIGHT
	static volatile int curr_frame = 0;
	if (curr_frame>=NUMBER_OF_SENSORS_FRAMES){
		curr_frame=0;
	}

	DMA1_Channel5->CMAR = (uint32_t)&sensors[curr_frame][0];

	curr_frame++;
#else
	DMA1_Channel5->CMAR = (uint32_t)sensors;
#endif
	DMA_SetCurrDataCounter(DMA1_Channel5,4);
	DMA_Cmd(DMA1_Channel5, ENABLE);
}
void DMA1_Channel5_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_HT5)==SET)
		{
			DMA_ClearFlag(DMA1_FLAG_HT5);
			GPIO_ResetBits(GPIOC, GPIO_Pin_3); // SS = 0
		}

	if (DMA_GetFlagStatus(DMA1_FLAG_TC5)==SET)
		{
		   	DMA_ClearFlag(DMA1_FLAG_TC5);
		    GPIO_SetBits(GPIOC, GPIO_Pin_3);
		}


}

void DMA2_Channel3_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	   if (DMA_GetFlagStatus(DMA2_FLAG_HT3)==SET)
	   {
		   	DMA_ClearFlag(DMA2_FLAG_HT3);
		    xSemaphoreGiveFromISR(xSoundBuffHTSemaphore, &xHigherPriorityTaskWoken );


	   }

	   if (DMA_GetFlagStatus(DMA2_FLAG_TC3)==SET)
	   {
		   	DMA_ClearFlag(DMA2_FLAG_TC3);
		    xSemaphoreGiveFromISR(xSoundBuffTCSemaphore, &xHigherPriorityTaskWoken );
	   }

}

#ifdef COLOR_LCD
void DMA2_Channel2_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	   if (DMA_GetFlagStatus(DMA2_FLAG_HT2)==SET)
	   {
		   	DMA_ClearFlag(DMA2_FLAG_HT2);
//		    xSemaphoreGiveFromISR(xSoundBuffHTSemaphore, &xHigherPriorityTaskWoken );


	   }

	   if (DMA_GetFlagStatus(DMA2_FLAG_TC2)==SET)
	   {
		   	DMA_ClearFlag(DMA2_FLAG_TC2);
//			DMA_Cmd(DMA2_Channel2, DISABLE);
		    xSemaphoreGiveFromISR(xColorLCD_DMA_Semaphore, &xHigherPriorityTaskWoken );
	   }

}

#endif

//*********************************************************************************************
//function ������������� wave-����                                                           //
//argument ��� �����                                                                         //
//result   0 - ��� ���������, ����� - ������                                                 //
//*********************************************************************************************
char wave_playback(const char *FileName)
{

//	FRESULT res;                                //��� ������������� ��������� ����������

	armadaSystem.wav_player.stop = false;
	DMA_DeInit(DMA2_Channel3);
	TIM_DeInit(TIM6);

	FIL file;    //�������� ������
	  UINT cnt;                                   //���������� ������� ����������� ����
	  uint32_t sampleRate;
	  uint16_t bitsPerSample;
	  volatile uint32_t subchunk2Size;//���������� ���� ������

	res = f_open( &file, FileName, FA_READ );   //������� ���� FileName ��� ������

	   if(res)//�� ������ ������� ����
	   {
		   f_close(&file);
	        // ������������ �����
	            Status = SD_Init();
	        // �������� ���������� � �����
	            SD_GetCardInfo(&SDCardInfo);
	                // ����� ����� � ��������� ������ ������
	                SD_SelectDeselect((uint32_t) (SDCardInfo.RCA << 16));
	           //     SD_SetDeviceMode(SD_POLLING_MODE);
	//                SD_SetDeviceMode(SD_INTERRUPT_MODE);
	                // � ��� ������� �� ������ � ������

//	                get_uid(request_text);
//	                xtea2_encipher(64,request_text,key_A);
//	                xtea2_decipher(64,request_text,key_A);

	                f_mount(0,&fs);

	                res = f_open( &file, FileName, FA_READ );   //������� ���� FileName ��� ������

	                if(res)//�� ������ ������� ����
	                {
	                	armadaSystem.wav_player.stop = true;
	                	return 1;
	                }
	   }
	//   res = f_lseek(&file,0x2c);                  //����������� ��������� �� ������ �������� ������

	   res = f_lseek(&file,24);

	   if(res)
	   {
		   f_close(&file);                             //������� ����
		   armadaSystem.wav_player.stop = true;
		   return 2;
	   }
	   f_read (&file,(uint32_t*)&sampleRate,4,&cnt);


	   if (cnt<4)
	   {
		   f_close(&file);                             //������� ����
		   armadaSystem.wav_player.stop = true;
		   return 3;
	   }
		   init_timer(sampleRate);
//	   sampleRate =*psampleRate;


	   res = f_lseek(&file,34);

	   if(res)
	   {
		   f_close(&file);                             //������� ����
		   armadaSystem.wav_player.stop = true;
		   return 2;
	   }
	   f_read (&file,(uint16_t*)&bitsPerSample,2,&cnt);

	   init_dma(bitsPerSample);
		res = f_lseek(&file,40);
		if(res)
		{
			f_close(&file);                             //������� ����
			armadaSystem.wav_player.stop = true;
			return 2;
		}
		f_read (&file,(uint32_t*)&subchunk2Size,4,&cnt);
		res = f_lseek(&file,0x2c);


		if(res)
		{
			f_close(&file);                             //������� ����
			armadaSystem.wav_player.stop = true;
			return 2;
		}

	   res=f_read (&file,&DAC_Buff[0],SOUND_BUFFER_SIZE*2,&cnt);       //��������� ����� ���� �������


	   if(res)
	   {
		   f_close(&file);                             //������� ����
//		   armadaSystem.wav_player.stop = true;
//		   xSemaphoreGive(xWavPlayerSemaphore);//
		   return 3;
	   }
	   if(subchunk2Size>cnt) subchunk2Size-=cnt;
	   else subchunk2Size = 0;


	   if(bitsPerSample==16)
	   {

		   for(uint16_t i=0;i < SOUND_BUFFER_SIZE; i++)
		   {
			   DAC_Buff[i] +=(uint16_t)0x8000;
			//   DAC_Buff[i] = DAC_Buff[i]>>4;
		   }

		   TIM_Cmd(TIM6, ENABLE);
	   while(1 /*subchunk2Size>0*/)
	    {

				 static uint16_t *pointer;
		   	   	 xSemaphoreTake( xSoundBuffHTSemaphore, portMAX_DELAY );

//		   	   	 taskENTER_CRITICAL();

		   		 f_read (&file,&DAC_Buff[0],SOUND_BUFFER_SIZE/*256*/,&cnt);    //��������� �� �������
//		   		 taskEXIT_CRITICAL();
		   		 if((cnt<SOUND_BUFFER_SIZE)||(armadaSystem.wav_player.stop))break;                        //���� ����� �����

//		   		 taskENTER_CRITICAL();
		   		 pointer = (uint16_t *)&DAC_Buff[0];
		   		 for(uint16_t i=0;i < SOUND_BUFFER_SIZE/2; i++)
		   			   {
		   			 	 	 *pointer +=(uint16_t)0x8000;
		   			 	 	 pointer ++;
		   			 //DAC_Buff[i] +=(uint16_t)0x8000;
		   			 //	 DAC_Buff[i] = DAC_Buff[i]>>4;
		   			   }
		   	//	 if(subchunk2Size>cnt) subchunk2Size-=cnt;
		   	//	 else break;//subchunk2Size = 0;

//		   		taskEXIT_CRITICAL();

		   		 xSemaphoreTake( xSoundBuffTCSemaphore, portMAX_DELAY );

//		   		taskENTER_CRITICAL();
	   	   	     f_read (&file,&DAC_Buff[SOUND_BUFFER_SIZE/2],SOUND_BUFFER_SIZE,&cnt);  //��������� �� �������
//	   	   	    taskEXIT_CRITICAL();

	   	   	     //���� ����� �����
   	   	     	 if((cnt<SOUND_BUFFER_SIZE)||(armadaSystem.wav_player.stop))break;                        //���� ����� �����
//   	   	     	 pointer = (uint16_t *)&DAC_Buff[SOUND_BUFFER_SIZE/2];

//	   	   	    taskENTER_CRITICAL();
	   	   	     for(uint16_t i=0;i < SOUND_BUFFER_SIZE/2; i++)
		   		 {
   	   	     		 	 *pointer +=(uint16_t)0x8000;
   	   	     		 	 pointer ++;
   	   	     		 // DAC_Buff[SOUND_BUFFER_SIZE/2+i] +=(uint16_t)0x8000;
			   	 	// DAC_Buff[SOUND_BUFFER_SIZE/2+i] = DAC_Buff[SOUND_BUFFER_SIZE/2+i]>>4;
			   	 }


 //		   		taskEXIT_CRITICAL();
   	   	  //   	 if(subchunk2Size>cnt) subchunk2Size-=cnt;
   	   	  //   	 else break;// subchunk2Size = 0;
	    }
	   } else {//���� 8 ���
		   TIM_Cmd(TIM6, ENABLE);
		   while(1/*subchunk2Size>0*/)
		  	    {

		  				 xSemaphoreTake( xSoundBuffHTSemaphore, portMAX_DELAY );
	//	  				taskENTER_CRITICAL();
		  				 f_read (&file,&DAC_Buff[0],SOUND_BUFFER_SIZE/*256*/,&cnt);    //��������� �� �������
//		  				taskEXIT_CRITICAL();
		  				 if((cnt<SOUND_BUFFER_SIZE)||(armadaSystem.wav_player.stop))break;                        //���� ����� �����
		 // 		   		 if(subchunk2Size>cnt) subchunk2Size-=cnt;
		 // 		   		 else subchunk2Size = 0;
		  	   	   	     xSemaphoreTake( xSoundBuffTCSemaphore, portMAX_DELAY );

//		  	   	   	taskENTER_CRITICAL();
		  	   	   	     f_read (&file,&DAC_Buff[SOUND_BUFFER_SIZE/2],SOUND_BUFFER_SIZE,&cnt);  //��������� �� �������
//		  	   		taskEXIT_CRITICAL();
		  	   	   	     //���� ����� �����
		     	   	     	 if((cnt<SOUND_BUFFER_SIZE)||(armadaSystem.wav_player.stop))break; //���� ����� �����
		  //   	   	     if(subchunk2Size>cnt) subchunk2Size-=cnt;
		  //   	   	     else subchunk2Size = 0;
		  	    }




	}


	   TIM_Cmd(TIM6, DISABLE);						//���������� ��������������

	   f_close(&file);                             //������� ����

/*
	   uint16_t i;
	   for(i=(uint16_t) DAC->DHR12L1; i>0; i-- )
	   {
		   DAC->DHR12L1 = i;//(uint16_t)0x8000/4;
	   }
*/

	armadaSystem.wav_player.stop= true;

	   xSemaphoreGive(xWavPlayerSemaphore);//

	   return 0;                                   //�������� ���������� �-��

}





void ir_tx_cursor_home(void){//������������� ������ � ������ ������
ir_tx_buffer_cursor.byte_pos = 0;
ir_tx_buffer_cursor.heder_been_sent = false;
ir_tx_buffer_cursor.bit_mask = (1<<7);
//ir_pulse_counter = IR_START; //��������� ����������
//ir_space_counter = IR_SPACE;// � ���������
}



bool get_buffer_bit(uint8_t index){		//��������� �������� ���� � ������ ��-���������
uint8_t byte_index;
uint8_t bit_index;
byte_index = index/8; //����������, � ����� ����� ��������� ������ ���
bit_index = index - (byte_index*8);//���������� ����� ���� � �����
if(rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
else return false;
}


bool get_zone_buffer_bit(TDamageZone zone, uint8_t index){		//��������� �������� ���� � ������ ��-���������
	uint8_t byte_index;
	uint8_t bit_index;
	byte_index = index/8; //����������, � ����� ����� ��������� ������ ���
	bit_index = index - (byte_index*8);//���������� ����� ���� � �����
	switch(zone)
	{
		case zone_1:
			{
				if(zone1_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
					else return false;
			}
		break;
		case zone_2:
			{
				if(zone2_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
					else return false;
			}
		break;
		case zone_3:
			{
				if(zone3_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
					else return false;
			}
		break;
		case zone_4:
			{
				if(zone4_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
					else return false;
			}
			break;
	}

}

void set_buffer_bit(uint8_t index, bool value){	//������ �������� ���� � ������ ��-���������
uint8_t byte_index;
uint8_t bit_index;
byte_index = index/8; //����������, � ����� ����� ��������� ������ ���
bit_index = index - (byte_index*8);//���������� ����� ���� � �����
if(value)
		{
			rx_buffer[byte_index] |= (1<<(7-bit_index));
		}
else	{
			rx_buffer[byte_index] &= ~(1<<(7-bit_index));
		}
}


void set_zone_buffer_bit (TDamageZone zone, uint8_t index, bool value){
	uint8_t byte_index;
	uint8_t bit_index;
	byte_index = index/8; //����������, � ����� ����� ��������� ������ ���
	bit_index = index - (byte_index*8);//���������� ����� ���� � �����


				switch(zone)
				{
					case zone_1:
					{
						if(value) zone1_rx_buffer[byte_index]|= (1<<(7-bit_index));

						else zone1_rx_buffer[byte_index] &= ~(1<<(7-bit_index));

					}
					break;
					case zone_2:
					{

						if(value) zone2_rx_buffer[byte_index]|= (1<<(7-bit_index));

						else zone2_rx_buffer[byte_index] &= ~(1<<(7-bit_index));
					}
					break;
					case zone_3:
					{

						if(value) zone3_rx_buffer[byte_index]|= (1<<(7-bit_index));

						else zone3_rx_buffer[byte_index] &= ~(1<<(7-bit_index));
					}
					break;
					case zone_4:
					{

						if(value) zone4_rx_buffer[byte_index]|= (1<<(7-bit_index));

						else zone4_rx_buffer[byte_index] &= ~(1<<(7-bit_index));
					}
					break;
				}

}

void init_var(void){//�������������� ����������

#if DEVICE_ROLE==BANDANA
bt_connect_hase_done=false;
#endif

#ifdef DIRECTION_CALCULATION
hit_mask = 0;
hit_timeout_counter = 0xFF;
#endif
	bt_configured=false;
	screen_auto_refresh=false;//��������� �������������� ���������� ������
#ifdef SENSORS_BECKLIGHT
	set_sensor_color(BLACK,0,zone_1);
	set_sensor_vibro(0,zone_1);
	set_sensor_color(BLACK,0,zone_2);
	set_sensor_vibro(0,zone_2);
	set_sensor_color(BLACK,0,zone_3);
	set_sensor_vibro(0,zone_3);
	set_sensor_color(BLACK,0,zone_4);
	set_sensor_vibro(0,zone_4);

#else
	sensors[zone_1] = BLACK;
	sensors[zone_2] = BLACK;
	sensors[zone_3] = BLACK;
	sensors[zone_4] = BLACK;
#endif
	countdown_shock_time=0;//�������� ������� ������� ����

	armadaSystem.battary.full_voltage = DEFAULT_BATTARY_FULL_VOLTAGE;
	armadaSystem.battary.low_voltage = DEFAULT_BATTARY_LOW_VOLTAGE;
	armadaSystem.player.health_after_start = DEFAULT_HEALTH;
	armadaSystem.player.health = armadaSystem.player.health_after_start;
	armadaSystem.player.team_color = DEFAULT_TEAM_COLOR;
	armadaSystem.player.id = DEFAULT_PLAYER_ID;
#if DEVICE_ROLE==TAG
	game_status = OUT_OF_GAME;
#else
	game_status = DEFAULT_PLAYER_STATUS;
#endif

	armadaSystem.player.shock_time = DEFAULT_SHOCK_TIME;

	armadaSystem.gun.ir_power = DEFAULT_IR_POWER;
	armadaSystem.gun.ir_power_offset = DEFAULT_IR_POWER_OFFSET;
	armadaSystem.gun.damage = DEFAULT_GUN_DAMAGE;
	armadaSystem.gun.clips_after_start = DEFAULT_CLIPS;
	armadaSystem.autostart_game = DEFAULT_AUTOSTART_GAME;
	armadaSystem.backlight_level = DEFAULT_BACKLIGHT_LEVEL;

#ifdef RTC_Enable
if(armadaSystem.autostart_game)
{
	armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
}
else
{
	armadaSystem.gun.clips = BKP_ReadBackupRegister(BKP_DR2);//armadaSystem.gun.clips_after_start;
}
#else
	armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
#endif
	armadaSystem.gun.rounds_placed_in_clip = DEFAULT_ROUNDS_IN_CLIP;
#ifdef RTC_Enable
if(armadaSystem.autostart_game)
{
	armadaSystem.gun.rounds = 0;
}
else
{
	armadaSystem.gun.rounds = BKP_ReadBackupRegister(BKP_DR1);//0;
}
#else
	armadaSystem.gun.rounds = 0;
#endif
	armadaSystem.gun.reload_time = DEFAULT_RELOAD_TIME;
	armadaSystem.gun.rate  = DEFAULT_RATE;
	armadaSystem.friendly_fire = DEFAULT_FRIENDLY_FIRE;
//	armadaSystem.wav_player.type_of_sound_to_play = NOTHING;


//	bool hit_in_processing = false;
//	armadaSystem.wav_player.stop = true;//��������� wav-�����
	safe_counter = 0;
	tim2_rec_state=REC_Idle;
	tim3_rec_state=REC_Idle;
	tim4_rec_state=REC_Idle;
	tim5_rec_state=REC_Idle;
	counter = 10;
	tim3_ir_rx_count = 0;
	tim2_ir_rx_count = 0;
	tim4_ir_rx_count = 0;
	tim5_ir_rx_count = 0;
	Zone1RxEvent = NOT_EVENT;
	Zone2RxEvent = NOT_EVENT;
	Zone3RxEvent = NOT_EVENT;
	Zone4RxEvent = NOT_EVENT;
	set_player_id(armadaSystem.player.id);
	set_team_color(armadaSystem.player.team_color/*Blue*/);
	set_gun_damage(armadaSystem.gun.damage);
	key_pressing_duration.key_1    =0;//�������� ��������
							  //������������
							  //������������ ������� ������
	key_pressing_duration.key_1_inc=1;//��������� ������ ������������
	key_pressing_duration.key_2    =0;//�������� ��������
							  //������������
							  //������������ ������� ������
	key_pressing_duration.key_2_inc=1;//��������� ������ ������������
	keyboard_event=no_key_pressing;
	reload_key_event = no_key_pressing;


}





/**************************************************************************************
* ������ ���������� "�������"
* ������������ ������ �� ������� ������ ����� ������ data_packet.data[0]
* � ��������� �������� ������
* ������� ���������� ����������  ������ ����� �������� ���� ������
***************************************************************************************/
void send_ir_shot_package(void){ //���������� ����� ("��������")

	FLASH_LED_ON;
	ir_tx_buffer_cursor.byte_pos = 0;
	ir_tx_buffer_cursor.bit_mask = (1<<7);
	ir_tx_buffer_cursor.bits_for_tx=14;//"�������" ������� �� 14 ���
	ir_tx_buffer_cursor.heder_been_sent = false;
	TIM_DeInit(TIM8);//���������� ��������� ������� �� ����������
//	TIM4->CCR1 = 50;
	init_pwm_TIM8();
}



/**************************************************************************************
* ��������� �������������� ������
* � �������� ��������� ������� ����������� ����������������� ����� ������ (�� 1 �� 127)
* � ���������� ���������� ������� � ���������� ���������� data_packet.player_id
* ����� ��������������� ������� ������������  data_packet.player_id.(bit_0 ... bit_7)
***************************************************************************************/
void set_player_id(uint8_t ID){
tx_buffer[0]= ID;
tx_buffer[0] &=~(1<<7);//������� ��� � �������� ������ ����� "0"

}



/**************************************************************************************
* ��������� �������������� (�����) �������
* � �������� ��������� ������� ����������� ����������������� ����� (����) ������� (�� 0 �� 3)
* � ���������� ���������� ������� � ���������� ���������� data_packet.team_id
* ����� ��������������� ������� ������������  data_packet.team_id.(bit_0 � bit_1)
***************************************************************************************/
void set_team_color(tteam_color  color){
tx_buffer[1] &=~((1<<7)|(1<<6));//�������� ��� ������� ����
tx_buffer[1] |=(color <<6);//������������� 6 � 7 ���� � ������������ � ������ �������
}


/**************************************************************************************
* ��������� ��������� ��������� ������ ������ (��������� ����)
* � �������� ��������� ������� ����������� ��������� ����
* � ���������� ���������� ������� � ���������� ���������� data_packet.damage
* ����� ��������������� ������� ������������  data_packet.damage.(bit_0 � bit_3)
***************************************************************************************/


void set_gun_damage(tgun_damage damage){
tx_buffer[1] &=~((1<<5)|(1<<4)|(1<<3)|(1<<2));//�������� ���� �����
tx_buffer[1] |=(damage << 2);
}







inline  TKEYBOARD_STATUS get_keyboard_status(int adc_value) {

volatile	TKEYBOARD_STATUS s_ret;



//adc_value = ADC_GetConversionValue(ADC1); // value from 0 to 4095i++;
if (adc_value < 1000) //���� ����� ����� (trigger)
	{
		s_ret=key_pressed ;
//		sensors[zone_1]=RED;

	}
else // ����� �� �����
	{
		s_ret=no_key_pressed;
//		sensors[zone_1]=BLACK;
	}
return s_ret;
}


TKEYBOARD_EVENT test_keyboard(int adc_value)//��������� ������� ����������
{
	TKEYBOARD_STATUS key_status;
	TKEYBOARD_EVENT ret_ev;
	key_status=get_keyboard_status(adc_value);
	switch(key_status)  //���������, ��� ������
	{
		case no_key_pressed:
		{
			if (key_pressing_duration.key_1>= SHORT_DURATION)
			{
				ret_ev=key_pressing;
                key_pressing_duration.key_1    =0;
                key_pressing_duration.key_1_inc=0;
				return ret_ev;

			}

			else
			{
				ret_ev=no_key_pressing;
                key_pressing_duration.key_1    =0;
                key_pressing_duration.key_1_inc=1;

			}

		}
		break;
	 	case key_pressed  : //������ ������ 1
		{
			if(key_pressing_duration.key_1>= SHORT_DURATION)
			{
				ret_ev=key_pressing;
             	key_pressing_duration.key_1    =0;
                key_pressing_duration.key_1_inc=0;
			}
			else
			{
				key_pressing_duration.key_1 += key_pressing_duration.key_1_inc;
                ret_ev=keyboard_event;

			}
		}
		break;
		default: ret_ev=keyboard_event;

	}

return   ret_ev;

}

TKEYBOARD_STATUS get_reload_key_status(int adc_value)//���������, ������ �� ������� "������������"
{
	volatile	TKEYBOARD_STATUS s_ret;

//	int adc_value;

//	adc_value = ADC_GetConversionValue(ADC1); // value from 0 to 4095i++;
	if (adc_value > 3000) //���� ����� ����� (reload)
		{
			s_ret=key_pressed ;

		}
	else // ����� �� �����
		{
			s_ret=no_key_pressed;
		}
	return s_ret;
}


TKEYBOARD_EVENT test_reload_key(int adc_value){//��������� ������� ������� "������������"
	TKEYBOARD_STATUS key_status;
		TKEYBOARD_EVENT ret_ev;
		key_status=get_reload_key_status(adc_value);
		switch(key_status)  //���������, ��� ������
		{
			case no_key_pressed:
			{
				if (key_pressing_duration.key_2>= SHORT_DURATION)
				{
					ret_ev=key_pressing;
	                key_pressing_duration.key_2    =0;
	                key_pressing_duration.key_2_inc=0;
					return ret_ev;

				}

				else
				{
					ret_ev=no_key_pressing;
	                key_pressing_duration.key_2    =0;
	                key_pressing_duration.key_2_inc=1;

				}

			}
			break;
		 	case key_pressed  : //������ ������ "������������"
			{
				if(key_pressing_duration.key_2>= SHORT_DURATION)
				{
					ret_ev=key_pressing;
	             	key_pressing_duration.key_2    =0;
	                key_pressing_duration.key_2_inc=0;
				}
				else
				{
					key_pressing_duration.key_2 += key_pressing_duration.key_2_inc;
	                ret_ev=reload_key_event;

				}
			}
			break;
			default: ret_ev=reload_key_event;

		}

	return   ret_ev;

}





void SPISend(uint16_t data) {

	GPIO_ResetBits(GPIOC, GPIO_Pin_3); // SS = 0
	SPI_I2S_SendData(SPI2, data);  // ��������� ������
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); // ���, ���� ������ �� ����������
    GPIO_SetBits(GPIOC, GPIO_Pin_3);
}

/*
bool get_buffer_bit(uint8_t index, TDamageZone zone){
	uint8_t byte_index;
	uint8_t bit_index;
	byte_index = index/8; //����������, � ����� ����� ��������� ������ ���
	bit_index = index - (byte_index*8);//���������� ����� ���� � �����
	switch (zone){
			case zone_1:{
				if(zone1_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
				else return false;
			}
			break;
			case zone_2:{
				if(zone2_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
				else return false;
				}
			break;
			case zone_3:{
				if(zone3_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
				else return false;
				}
			break;
			case zone_4:{
				if(zone4_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
				else return false;
				}
				break;

			}


}

*/

tir_message get_ir_message_from_buffer(TDamageZone zone) //��������� �� ������ �� ��������� ���������� ���������
{
	tir_message result;
	switch (zone){
		case zone_1:{
			result.ID = zone1_rx_buffer[0];//��� ������� � ������ �������� ����� (������ 0)
			result.param = zone1_rx_buffer[1];//��������� ������� �� ������ �������� ����� (������ 1)
			result.control_byte = zone1_rx_buffer[2];//����������� ����
			return result;
		}
		break;
		case zone_2:{
			result.ID = zone2_rx_buffer[0];//��� ������� � ������ �������� ����� (������ 0)
			result.param = zone2_rx_buffer[1];//��������� ������� �� ������ �������� ����� (������ 1)
			result.control_byte = zone2_rx_buffer[2];//����������� ����
			return result;
			}
		break;
		case zone_3:{
			result.ID = zone3_rx_buffer[0];//��� ������� � ������ �������� ����� (������ 0)
			result.param = zone3_rx_buffer[1];//��������� ������� �� ������ �������� ����� (������ 1)
			result.control_byte = zone3_rx_buffer[2];//����������� ����
			return result;
			}
		break;
		case zone_4:{
			result.ID = zone4_rx_buffer[0];//��� ������� � ������ �������� ����� (������ 0)
			result.param = zone4_rx_buffer[1];//��������� ������� �� ������ �������� ����� (������ 1)
			result.control_byte = zone4_rx_buffer[2];//����������� ����
			return result;
			}
			break;

		}

}


trx_packet get_packet_value(TDamageZone zone){ //��������� ������ �� ����������� ������
trx_packet result;
uint8_t byte_tmp;

switch (zone){
	case zone_1:{
		result.player_id = zone1_rx_buffer[0];
		byte_tmp = zone1_rx_buffer[1];
		byte_tmp = byte_tmp << 2; //����������� �� ��� ����� �������
		byte_tmp = byte_tmp >> 4;
		result.damage = damage_value[byte_tmp];
		result.team_id = zone1_rx_buffer[1]>>6;
		return result;
	}
	break;
	case zone_2:{
			result.player_id = zone2_rx_buffer[0];
			byte_tmp = zone2_rx_buffer[1];
			byte_tmp = byte_tmp << 2; //����������� �� ��� ����� �������
			byte_tmp = byte_tmp >> 4;
			result.damage = damage_value[byte_tmp];
			result.team_id = zone2_rx_buffer[1]>>6;
			return result;
		}
	break;
	case zone_3:{
			result.player_id = zone3_rx_buffer[0];
			byte_tmp = zone3_rx_buffer[1];
			byte_tmp = byte_tmp << 2; //����������� �� ��� ����� �������
			byte_tmp = byte_tmp >> 4;
			result.damage = damage_value[byte_tmp];
			result.team_id = zone3_rx_buffer[1]>>6;
			return result;
		}
	break;
	case zone_4:{
			result.player_id = zone4_rx_buffer[0];
			byte_tmp = zone4_rx_buffer[1];
			byte_tmp = byte_tmp << 2; //����������� �� ��� ����� �������
			byte_tmp = byte_tmp >> 4;
			result.damage = damage_value[byte_tmp];
			result.team_id = zone4_rx_buffer[1]>>6;
			return result;
		}
		break;

	}
}

/*
void Sinus_calculate()
{
  for(uint16_t i=0; i<360; i++)
  {
    Sinus[i] = (uint16_t) lround((sin(i*(pi/180)) + 1)*4095/2);
  }
}
*/


bool stopWavPlayer(void){

	if ( armadaSystem.wav_player.stop) return true;//����� ��� ����������
	else armadaSystem.wav_player.stop = true;//������������� �����
	if (xSemaphoreTake( xWavPlayerSemaphore, (portTickType)600/*50*/ /*portMAX_DELAY*/)== pdTRUE)//���, ���� ����� �����������
	{
		return true;//�������� �������, ����� ��������� �����������
	}
	else {
		return false;//�� 50 ����� ����� �� ����������� (������� �� �������)
	}
}


void hit_processing(TDamageZone zone)//������������ ���������
{


	trx_hit hit;
	trx_packet rx_pack;
	hit.time_stamp = xTaskGetTickCount();
	rx_pack = get_packet_value(zone);

	if(game_status==OUT_OF_GAME)return;//����� ��� ����, ���������� �����
	if((armadaSystem.player.team_color==rx_pack.team_id)&&(armadaSystem.friendly_fire==false)) return;//����� �� ������ ����� �� �������, � ������������� ����� ������� - ���������� �����
	hit.rx_package = rx_pack;
//	sensors[zone]=WHITE;
	if (xSemaphoreTake( xHitSemaphore, (portTickType) HIT_PROCESSING_TIMEOUT /*portMAX_DELAY*/)== pdTRUE)//���, ���� ����� �����������
		{//�������� �������

//		taskENTER_CRITICAL();
		{
		    // ����� ��� ����� ���, ���������� �������� �� ������ ���� ��������.
		if(safe_counter==0) //���� ��� ��������������� ���������
			{
				safe_counter=SAFE_TIME;
				hit_in_processing = hit;

				if(armadaSystem.player.health > hit.rx_package.damage)//���� ������, ��� ������� ��������
				{
					armadaSystem.player.health =  armadaSystem.player.health - hit.rx_package.damage;
					armadaSystem.wav_player.type_of_sound_to_play = HIT;//���� ������������� ���� �������


#if DEVICE_ROLE==BANDANA
					bt_tag_init_with_game_status(game_status, HEALTH_UPDATE);
#elif DEVICE_ROLE==TAG

#else
					send_package_by_bluetooth(hit.rx_package);
#endif
					xSemaphoreGive(xWavPlayerManagerSemaphore);
				}
				else {
					armadaSystem.player.health = 0;
					game_status = OUT_OF_GAME;
					armadaSystem.wav_player.type_of_sound_to_play = GAME_OVER;//���� ������������� ���� "���� ���������"
#if DEVICE_ROLE==BANDANA
					bt_tag_init_with_game_status(OUT_OF_GAME,STOP_GAME);
#elif DEVICE_ROLE==TAG

#else
					send_package_by_bluetooth(hit.rx_package);
#endif
					if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)
					{


#ifndef 	COLOR_LCD
//����� ����� �������� ��� ������ �������� "GAME_OVER" ��� �����-������ ���������

						clear_screen();
						drawBitmap(0,0,game_over_black_image,84,48,1);
						if (!screen_auto_refresh)lcd8544_dma_refresh();

#else


						if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
					 {



							drawBMPfromFlashUseDMA(game_over_pic,0,0,128,128,Rotation_0);


						xSemaphoreGive(xColorLCDSemaphore);
					 }

#endif







					}


					 if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//���� ���������� �����������
			        {


#ifndef COLOR_LCD
			    		drawDigit(52+2,0,10,1);//������ �������� ������
#else
			    		if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
						{
							drawBMPfromFlash(bluetooth1,16,32);
							xSemaphoreGive(xColorLCDSemaphore);
						}
#endif
					}
					/*
					volatile tsystem_event_type sys_event_tmp;
					sys_event_tmp.event_source = BLUETOOTH;
					xQueueSendToBack(xEventQueue, &sys_event_tmp, (portTickType)(TIC_FQR));//�� ������� �������� ������ ������ ����������
*/
					xSemaphoreGive(xWavPlayerManagerSemaphore);


				}
if(game_status != OUT_OF_GAME)
{
#ifndef 	COLOR_LCD
	  lcd_health_update();
#else
	  color_lcd_health_update();
#endif
}




			}
		}
//		taskEXIT_CRITICAL();
		xSemaphoreGive(xHitSemaphore);//������ �������
		if (compareHits(hit,hit_in_processing))//���� �������������� ��� �� �����
		{
		//	 portTickType xLastWakeTime;

#ifdef DIRECTION_CALCULATION
taskENTER_CRITICAL();
hit_mask |= (1<<zone);
hit_timeout_counter=0;
taskEXIT_CRITICAL();
#endif

			//������� ���������
#ifdef SENSORS_BECKLIGHT
			set_sensor_color(index_to_color[hit.rx_package.team_id]|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES,zone);
#ifndef DIRECTION_CALCULATION
			set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone);
#endif
#else
			sensors[zone]=index_to_color[hit.rx_package.team_id]|VIBRO_HP;
#endif
			//�������������� ���������� xLastWakeTime ������� ��������.
		//	xLastWakeTime = xTaskGetTickCount ();
			vTaskDelay(TIC_FQR);//�� ������� ������� ��������� ����
//			vTaskDelayUntil( &xLastWakeTime, TIC_FQR );
//			sensors[zone]=index_to_color[rx_pack.team_id];
//			vTaskDelay(TIC_FQR/2);
#ifdef SENSORS_BECKLIGHT
			set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone);
			set_sensor_vibro(0,zone);
#else
			sensors[zone]=BLACK;
#endif
		}


//			return true;
		}
		else {
//			vTaskDelay(TIC_FQR);
//			sensors[zone]=BLACK;
		//	return false;//�� 50 ����� ����� �� ����������� (������� �� �������)
		}
/*

		       		   xLastWakeTime = xTaskGetTickCount();

		       		   vTaskDelayUntil( &xLastWakeTime, ( TIC_FQR) );
		       		   //

*/
}


void message_processing(TDamageZone zone)//������������ ��������� (������� ������)
{
	trx_captured_message cap_message;
	tir_message ir_message;
	cap_message.time_stamp = xTaskGetTickCount();
	if (get_zone_buffer_bit(zone,0))//���� ���� ��� ����� 1, �� ��� ����� � ��������
	{
		ir_message = get_ir_message_from_buffer(zone);
		cap_message.rx_message = ir_message;
		if (xSemaphoreTake( xHitSemaphore, (portTickType) HIT_PROCESSING_TIMEOUT /*portMAX_DELAY*/)== pdTRUE)//���, ���� ����� �����������
		{//�������� �������
			taskENTER_CRITICAL();
			{
				if (!compareMessages(cap_message, message_in_processing))
				{
					message_in_processing = cap_message;

					switch(cap_message.rx_message.ID)//���� ��� �������
               		{
                   		case Add_Health: //�������� "�����"
                   		{
                   			//��� ��� ���������� �����
					        break;
                   		}
					    case Add_Rounds://�������� "��������"
					    {
					 		//��� ��� ���������� ��������
                      		break;
                   		}
                   		case Change_color:
						{
							//��� ��� ����� �����
							if((cap_message.rx_message.param>=0)&&(cap_message.rx_message.param<=3))
							{
								armadaSystem.player.team_color=cap_message.rx_message.param;
								set_team_color(armadaSystem.player.team_color);
								armadaSystem.wav_player.type_of_sound_to_play = MESSAGE;
								switch(armadaSystem.player.team_color)
								{
									case Red:
									{
										memcpy(armadaSystem.wav_player.message_sound_file_name,"nowred.wav",strlen("nowred.wav"));
										armadaSystem.wav_player.message_sound_file_name[strlen("nowred.wav")] = 0;
										//armadaSystem.wav_player.message_sound_file_name = ;
									}
									break;
									case Blue:
									{
										memcpy(armadaSystem.wav_player.message_sound_file_name,"nowblue.wav",strlen("nowblue.wav"));
										armadaSystem.wav_player.message_sound_file_name[strlen("nowblue.wav")] = 0;
										//armadaSystem.wav_player.message_sound_file_name = "nowblue.wav";
									}
									break;
									case Yellow:
									{
										memcpy(armadaSystem.wav_player.message_sound_file_name,"nowyell.wav",strlen("nowyell.wav"));
										armadaSystem.wav_player.message_sound_file_name[strlen("nowyell.wav")] = 0;
										//armadaSystem.wav_player.message_sound_file_name = "nowyellow.wav";
									}
									break;
									case Green:
									{
										memcpy(armadaSystem.wav_player.message_sound_file_name,"nowgreen.wav",strlen("nowgreen.wav"));
										armadaSystem.wav_player.message_sound_file_name[strlen("nowgreen.wav")] = 0;
										//armadaSystem.wav_player.message_sound_file_name = "nowgreen.wav";
									}
									break;
									default:
									{

									}
									break;

								}
//								save_parameters_to_sd_card();
								xSemaphoreGive(xWavPlayerManagerSemaphore);
								taskEXIT_CRITICAL();
#ifdef SENSORS_BECKLIGHT
								set_sensor_color(index_to_color[armadaSystem.player.team_color]|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES,zone_1);
								set_sensor_color(index_to_color[armadaSystem.player.team_color]|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES,zone_2);
								set_sensor_color(index_to_color[armadaSystem.player.team_color]|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES,zone_3);
								set_sensor_color(index_to_color[armadaSystem.player.team_color]|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES,zone_4);
#else

								sensors[zone_1]=index_to_color[armadaSystem.player.team_color]&~(VIBRO_HP | VIBRO_LP);
								sensors[zone_2]=index_to_color[armadaSystem.player.team_color]&~(VIBRO_HP | VIBRO_LP);
								sensors[zone_3]=index_to_color[armadaSystem.player.team_color]&~(VIBRO_HP | VIBRO_LP);
								sensors[zone_4]=index_to_color[armadaSystem.player.team_color]&~(VIBRO_HP | VIBRO_LP);

#endif
								vTaskDelay(TIC_FQR);
#ifdef SENSORS_BECKLIGHT
								set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone_1);
								set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone_2);
								set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone_3);
								set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone_4);
#else

								sensors[zone_1]=BLACK;
								sensors[zone_2]=BLACK;
								sensors[zone_3]=BLACK;
								sensors[zone_4]=BLACK;

#endif
								taskENTER_CRITICAL();

							}
							else
							{
								//������ ����� �����

							}
							break;
						}
						case Command://����� �� �������������� �������
					    {
							switch(cap_message.rx_message.param)//�������, ����� ��� �������
							{
								case 0x05://������ ����� ���� ����������
								{
									taskEXIT_CRITICAL();

									armadaSystem.player.health = armadaSystem.player.health_after_start;
#if DEVICE_ROLE == BANDANA
									bandana_game_session_ID = (uint32_t) xTaskGetTickCount();
									bt_tag_init_with_game_status(true, NEW_GAME);
#else
									send_message_by_bluetooth(cap_message.rx_message);
#endif


									/*
#ifndef 	COLOR_LCD
	  lcd_health_update();
#else
	  color_lcd_health_update();
#endif
*/
									armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
/*
#ifndef 	COLOR_LCD
									lcd_clips_update();
#else
									color_lcd_clips_update();

#endif
*/
									armadaSystem.gun.rounds = 0;

/*
#ifndef 	COLOR_LCD
									lcd_rounds_update();
									if (!screen_auto_refresh) lcd8544_dma_refresh();

#else
	  color_lcd_rounds_update();
#endif
*/
									initGUI();
									xSemaphoreGive(xGameOverSemaphore);//������ �������
	  	  	  	  	  	  	  	  	armadaSystem.wav_player.type_of_sound_to_play = START_GAME;
									xSemaphoreGive(xWavPlayerManagerSemaphore);
//									taskEXIT_CRITICAL();
#ifdef SENSORS_BECKLIGHT
									set_sensor_color(RED|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_1);
									set_sensor_color(RED|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_2);
									set_sensor_color(RED|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_3);
									set_sensor_color(RED|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_4);
#else


									sensors[zone_1]=RED;
									sensors[zone_2]=RED;
									sensors[zone_3]=RED;
									sensors[zone_4]=RED;

#endif
									vTaskDelay(TIC_FQR/3);
#ifdef SENSORS_BECKLIGHT
									set_sensor_color(BLUE|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_1);
									set_sensor_color(BLUE|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_2);
									set_sensor_color(BLUE|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_3);
									set_sensor_color(BLUE|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_4);
#else

									sensors[zone_1]=BLUE;
									sensors[zone_2]=BLUE;
									sensors[zone_3]=BLUE;
									sensors[zone_4]=BLUE;

#endif
									vTaskDelay(TIC_FQR/3);
#ifdef SENSORS_BECKLIGHT
									set_sensor_color(GREEN|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_1);
									set_sensor_color(GREEN|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_2);
									set_sensor_color(GREEN|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_3);
									set_sensor_color(GREEN|HIGH_BRIDHTNESS,NUMBER_OF_SENSORS_FRAMES, zone_4);
#else

									sensors[zone_1]=GREEN;
									sensors[zone_2]=GREEN;
									sensors[zone_3]=GREEN;
									sensors[zone_4]=GREEN;

#endif
									vTaskDelay(TIC_FQR/3);
#ifdef SENSORS_BECKLIGHT
									set_sensor_color(BLACK, 0, zone_1);
									set_sensor_color(BLACK, 0, zone_2);
									set_sensor_color(BLACK, 0, zone_3);
									set_sensor_color(BLACK, 0, zone_4);

									vTaskDelay(TIC_FQR/3);
									set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_1);
									set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_2);
									set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_3);
									set_sensor_color(index_to_color[armadaSystem.player.team_color], armadaSystem.backlight_level, zone_4);
#else


									sensors[zone_1]=BLACK;
									sensors[zone_2]=BLACK;
									sensors[zone_3]=BLACK;
									sensors[zone_4]=BLACK;

#endif
							taskENTER_CRITICAL();
									game_status = true;
//									xSemaphoreGive(xGameOverSemaphore);//������ �������
									break;
								}
								case 0x00://"���������" ������
								{

									break;
								}
								default: break;
						}
						break;
               		}
           		}
			}

		}
			taskEXIT_CRITICAL();
			xSemaphoreGive(xHitSemaphore);//������ �������
		}
	}
}

bool get_settings_from_ini_file(void)//��������� ������� ��
{
//	FRESULT res;
	 res = f_open( &fsrc , "0:/armada.ini" ,/* FA_CREATE_NEW*/FA_READ /*| FA_WRITE*/);
	    if ( res == FR_OK )
	    {
	    	while (f_gets(&readBuffer[0],256,&fsrc)/*readln(&string_buffer[0])*/)
	    	{
	    		parsing_string(&readBuffer[0]);
	    	}
	    	f_close(&fsrc);
	    	return true;
	    }
	    else   return false;
}


void parsing_string(char* record)//������ ������
{
	if (*record =='#') return;
	switch(get_parameter_index(record))
	{
		case 0:{get_int_argument_value(record,&armadaSystem.player.id,0,127);} break;
		case 1:{get_int_argument_value(record,&armadaSystem.player.team_color,0,3);} break;
		case 2:{get_string_argument_value(record,armadaSystem.wav_player.shot_sound_file_name);} break;
		case 3:{get_string_argument_value(record,armadaSystem.wav_player.hit_sound_file_name);} break;
		case 4:{get_int_argument_value(record,&armadaSystem.friendly_fire,0,1);} break;
		case 5:{get_int_argument_value(record,&armadaSystem.gun.damage,0,15);} break;
		case 6:{get_string_argument_value(record,armadaSystem.bluetooth.name);} break;
		case 7:{
					get_string_argument_value(record,armadaSystem.bluetooth.mac_adress_for_bind);
					trim_mac_adress(armadaSystem.bluetooth.mac_adress_for_bind);
					convert_mac_adress(armadaSystem.bluetooth.mac_adress_for_bind);
				} break;
		case 8:{get_double_word_argument_value(record,&armadaSystem.bluetooth.baudrate,110,921600);} break;
		case 9:{get_string_argument_value(record,armadaSystem.bluetooth.role);} break;
		case 10:{get_string_argument_value(record,armadaSystem.bluetooth.cmode);} break;
		case 11:{get_string_argument_value(record,armadaSystem.bluetooth.pswd);} break;
		case 12:{get_string_argument_value(record,armadaSystem.wav_player.empty_clip_sound_file_name);} break;
		case 13:{get_string_argument_value(record,armadaSystem.wav_player.reload_sound_file_name);} break;
		case 14:{get_word_argument_value(record,&armadaSystem.gun.reload_time,0,1000);} break;
		case 15:{get_word_argument_value(record,&armadaSystem.gun.rate,30,1000);} break;
		case 16:{get_word_argument_value(record,&armadaSystem.battary.full_voltage,2800,12000);} break;
		case 17:{get_word_argument_value(record,&armadaSystem.battary.low_voltage,2000,3600);} break;
		case 18:{get_int_argument_value(record,&armadaSystem.gun.ir_power,0,127);} break;
		case 19:{get_string_argument_value(record,armadaSystem.wav_player.start_game_sound_file_name);} break;
		case 20:{get_word_argument_value(record,&armadaSystem.gun.clips_after_start,0,1000);} break;
		case 21:{get_word_argument_value(record,&armadaSystem.gun.rounds_placed_in_clip,0,1000);} break;
		case 22:{get_int_argument_value(record,&armadaSystem.autostart_game,0,1);} break;
		case 23:{get_int_argument_value(record,&armadaSystem.backlight_level,0,NUMBER_OF_SENSORS_FRAMES);} break;
		case 24:{get_int_argument_value(record,&armadaSystem.gun.ir_power_offset,0,30);} break;
		case 25:{get_string_argument_value(record,armadaSystem.wav_player.sonar_sound_file_name);} break;
		default: break;
	}
}

uint8_t get_parameter_index(char* record)//��������, ��� �� ��������
{
	volatile char* cursor_pos;
	uint8_t param_len;
	uint8_t record_len;

	for (int index=0; index <(sizeof(parametersPointers)/sizeof(char*)); index++)
	{
		param_len = strlen(parametersPointers[index]);
		record_len = strlen(record);
		if (memmem(record,record_len,parametersPointers[index],param_len)!=NULL)
		{
			return index;
		}
	}
	return 0xff;
}




bool save_parameters_to_sd_card(void){
	res = f_open( &fsrc , "0:/system.bin" , FA_CREATE_NEW | FA_WRITE);
    if ( res == FR_OK )
    {
       /* Write buffer to file */
       res = f_write(&fsrc, &armadaSystem, sizeof(armadaSystem), &br);

		    //            	  printf("Demo.TXT successfully created        \r\n");

		                      /*close file */
		                      f_close(&fsrc);
		                      if(res == FR_OK) return true;
		                      else return false;
		                    }
		                    else if ( res == FR_EXIST )
		                    {
		                    	UINT cnt;
		                    	res = f_open( &fsrc, "0:/system.bin", /*FA_READ*/  FA_WRITE);
		                    	res = f_write(&fsrc, &armadaSystem, sizeof(armadaSystem), &br);
		                    			    //            	  printf("Demo.TXT successfully created        \r\n");
		                      /*close file */
		                    	f_close(&fsrc);
		                    	if(res == FR_OK) return true;
		                    	 else return false;
		                    }
}




bool read_parameters_from_sd_card(void)
{

	UINT cnt;
	res = f_open( &fsrc , "0:/system.bin" ,FA_READ);



	if ( res == FR_OK )
	  {
		res = f_read (&fsrc,&armadaSystem,sizeof(armadaSystem),&cnt);
		 f_close(&fsrc);
		 if (res== FR_OK) return true;
		 else return false;
	  }
	 else return false;
}

extern volatile unsigned char usart1_rxBufHead;
bool send_set_at_command(char* cmd, char* param){
	USART1_FlushTxBuf(); //�������� ���������� �����
	USART1_FlushRxBuf(); //�������� �������� �����
	xSemaphoreTake(xUsart1Semaphore, 300);//������� �������
	USART1_SendStr(cmd);
	USART1_SendStr("=");
	USART1_SendStr(param);
	USART1_SendStr("\r\n");
	if(xSemaphoreTake(xUsart1Semaphore, USART_PROCESSING_TIMEOUT )== pdTRUE)//���� ������� ������ ������
	{
		if (memmem(&usart1_RxBuf[usart1_rxBufHead],USART1_GetRxCount(),"OK",2)!=NULL)
				{
					return true;
				}
		else
				{

					return false;
				}
	}
	else
	{
		return false;
	}
}

bool send_test_at_command(void)
{
	USART1_FlushTxBuf(); //�������� ���������� �����
		USART1_FlushRxBuf(); //�������� �������� �����
		xSemaphoreTake(xUsart1Semaphore, 30);//������� �������
		USART1_SendStr("AT");
		USART1_SendStr("\r\n");
		if(xSemaphoreTake(xUsart1Semaphore, USART_PROCESSING_TIMEOUT )== pdTRUE)//���� ������� ������ ������
		{
			if (memmem(&usart1_RxBuf[usart1_rxBufHead],USART1_GetRxCount(),"OK",2)!=NULL)
					{
						return true;
					}
			else
			{
				lcd8544_putstr(0, 32, "��� OK ", 0); // ����� ������ ������
				if (!screen_auto_refresh) lcd8544_dma_refresh(); // ����� ������ �� ����� ! ��� ����� ������ ����� �� ����� !
				vTaskDelay(600);
				return false;
			}
		}
		else
		{
			  lcd8544_putstr(0, 32, "��� �������� ", 0); // ����� ������ ������
			  if (!screen_auto_refresh) lcd8544_dma_refresh(); // ����� ������ �� ����� ! ��� ����� ������ ����� �� ����� !
			  vTaskDelay(600);
			  return false;
		}
}
void send_package_by_bluetooth(trx_packet package)
{
  uint8_t mask = (1<<7);
  USART1_FlushTxBuf(); //�������� ���������� �����
  USART1_SendStr("h");
  while (mask)
  {
	  if ((package.player_id)&(mask)) USART1_SendStr("1");
	  else USART1_SendStr("0");
	  mask = mask >> 1;
  }

  mask = 1<<1;
  while (mask)
   {
 	  if ((package.team_id)&(mask)) USART1_SendStr("1");
 	  else USART1_SendStr("0");
 	  mask = mask >> 1;
   }
  volatile uint8_t damage_index;
  damage_index = get_damage_index(package.damage);
  mask = 1<<3;
  while (mask)
   {
 	  if ((damage_index)&(mask))  USART1_SendStr("1");
 	  else USART1_SendStr("0");
 	  mask = mask >> 1;
   }
  USART1_SendStr("t");
}






bool configure_bluetooth(void)//����������� ������ ������
{

	bool at_res;
	unsigned char line_counter=0;
	unsigned char line_on_screen=display_vertical_screen_resolution()/display_standard_symbol_height();


	USART_DeInit(USART1);
	InitCOM1(38400);
	bt_set_at_commands_mode(true);
	 vTaskDelay(100);
	 bt_reset();
	 BL_ON;
	display_put_string(0,line_counter++, "Set BT");

	 at_res = send_test_at_command();

	 if(!at_res){
		 if(!(line_counter<line_on_screen)){
		 	 vTaskDelay(150);
		 	 line_counter=0;
		 	 display_clear_screen();
		 }
		 display_put_string(0,display_standard_symbol_height()*(line_counter++),"38400: Test error");

		 USART_DeInit(USART1);
		 InitCOM1(9600);
		 vTaskDelay(100);
	 }
	if(strlen(armadaSystem.bluetooth.name))//���� ������ � ������ �� ������
	{

		 if(!((line_counter+1)<line_on_screen)){
		 	 vTaskDelay(150);
		 	 line_counter=0;
		 	 display_clear_screen();
		 }
		at_res = send_set_at_command(at_name,armadaSystem.bluetooth.name);
		display_put_string(0,display_standard_symbol_height()*(line_counter),"Name: ");
		display_put_string(display_standard_symbol_width()*5,display_standard_symbol_height()*(line_counter++),armadaSystem.bluetooth.name);

		if (at_res)
			{
				display_put_string(0,display_standard_symbol_height()*(line_counter++),"OK");
			}
		else
			{
				display_put_string(0,display_standard_symbol_height()*(line_counter++),"ERROR");
			}
	}
	if(strlen(armadaSystem.bluetooth.cmode))//���� ������ � ������ �� ������
	{

		if(!((line_counter+1)<line_on_screen)){
		 	 vTaskDelay(150);
		 	 line_counter=0;
		 	 display_clear_screen();
		 }

		at_res = send_set_at_command(at_cmode,armadaSystem.bluetooth.cmode);
		display_put_string(0,display_standard_symbol_height()*(line_counter),at_cmode);
		display_put_string(display_standard_symbol_width()*8,display_standard_symbol_height()*(line_counter),"=");
		display_put_string(display_standard_symbol_width()*9,display_standard_symbol_height()*(line_counter++),armadaSystem.bluetooth.cmode);
		if (at_res)
					{
						display_put_string(0,display_standard_symbol_height()*(line_counter++),"OK");
					}
				else
					{
						display_put_string(0,display_standard_symbol_height()*(line_counter++),"ERROR");
					}
	}
	if(strlen(armadaSystem.bluetooth.role))//���� ������ � ������ �� ������
	{
		if(!((line_counter+1)<line_on_screen)){
		 	 vTaskDelay(150);
		 	 line_counter=0;
		 	 display_clear_screen();
		 }
		at_res = send_set_at_command(at_role,armadaSystem.bluetooth.role);
		display_put_string(0,display_standard_symbol_height()*(line_counter),at_role);
		display_put_string(display_standard_symbol_width()*7,display_standard_symbol_height()*(line_counter),"=");
		display_put_string(display_standard_symbol_width()*8,display_standard_symbol_height()*(line_counter++),armadaSystem.bluetooth.role);
		if (at_res)
					{
						display_put_string(0,display_standard_symbol_height()*(line_counter++),"OK");
					}
				else
					{
						display_put_string(0,display_standard_symbol_height()*(line_counter++),"ERROR");
					}
	}
	if(strlen(armadaSystem.bluetooth.mac_adress_for_bind))//���� ������ � ������ �� ������
	{
		if(!((line_counter+1)<line_on_screen)){
		 	 vTaskDelay(150);
		 	 line_counter=0;
		 	 display_clear_screen();
		 }
		at_res = send_set_at_command(at_bind,armadaSystem.bluetooth.mac_adress_for_bind);
		display_put_string(0,display_standard_symbol_height()*(line_counter),"BIND:");
		display_put_string(display_standard_symbol_width()*5,display_standard_symbol_height()*(line_counter++),armadaSystem.bluetooth.mac_adress_for_bind);
		if (at_res)
			{
				display_put_string(0,display_standard_symbol_height()*(line_counter++),"OK");
			}
		else
			{
				display_put_string(0,display_standard_symbol_height()*(line_counter++),"ERROR");
			}
	}
	if(strlen(armadaSystem.bluetooth.pswd))//���� ������ � ������ �� ������
		{

		if(!((line_counter+1)<line_on_screen)){
		 	 vTaskDelay(150);
		 	 line_counter=0;
		 	 display_clear_screen();
		 }
			at_res = send_set_at_command(at_pswd,armadaSystem.bluetooth.pswd);
			display_put_string(0,display_standard_symbol_height()*(line_counter),"PASSWORD:");
			display_put_string(display_standard_symbol_width()*9,display_standard_symbol_height()*(line_counter++),armadaSystem.bluetooth.pswd);
			if (at_res)
			{
				display_put_string(0,display_standard_symbol_height()*(line_counter++),"OK");
			}
			else
			{
				display_put_string(0,display_standard_symbol_height()*(line_counter++),"ERROR");
			}
		}
	bt_set_at_commands_mode(false);
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);//
	USART_DeInit(USART1);
	if(armadaSystem.bluetooth.baudrate) InitCOM1(armadaSystem.bluetooth.baudrate);
	else InitCOM1(BAUDRATE);
//	bt_reset();
	vTaskDelay(TIC_FQR*2);
#ifndef 	COLOR_LCD
	BL_OFF;
#endif
}

void bt_set_at_commands_mode(bool mode)//��������� ������ ������ � ����� at-������
{
	if (mode) GPIO_SetBits(GPIOB, GPIO_Pin_10);
	else GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}


void bt_reset(void)//���������� ����� ������-������
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_4);
	 vTaskDelay(200);
	GPIO_SetBits(GPIOC, GPIO_Pin_4);
	 vTaskDelay(200);
}
uint8_t get_damage_index(uint8_t damage)
{
	if ((damage==1)||(damage==0))	return	0b00000000;
	if (damage==2) 					return	0b00000001;
	if ((damage==3)||(damage==4)) 	return 	0b00000010;
	if (damage==5) 					return 	0b00000011;
	if ((damage==6)||(damage==7)) 	return 	0b00000100;
	if ((damage>7)&&(damage<=10)) 	return 	0b00000101;
	if ((damage>10)&&(damage<=15)) 	return 	0b00000110;
	if ((damage>15)&&(damage<=17)) 	return 	0b00000111;
	if ((damage>17)&&(damage<=20)) 	return 	0b00001000;
	if ((damage>20)&&(damage<=25)) 	return 	0b00001001;
	if ((damage>25)&&(damage<=30)) 	return 	0b00001010;
	if ((damage>30)&&(damage<=35)) 	return 	0b00001011;
	if ((damage>35)&&(damage<=40)) 	return 	0b00001100;
	if ((damage>40)&&(damage<=50)) 	return 	0b00001101;
	if ((damage>50)&&(damage<=75)) 	return 	0b00001110;
	if ((damage>75)&&(damage<=100)) return 	0b00001111;
	return 	0b00001111;


}


void ADC1_2_IRQHandler(void)
{
int ADC_result;
	if(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC))
	{
		ADC_ClearFlag(ADC2, ADC_FLAG_EOC);
		ADC_result = ADC_GetConversionValue(ADC2);
	}
	if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
	{
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
		ADC_result = ADC_GetConversionValue(ADC1);
	}
}

extern unsigned char lcd8544_buff[84*6]; // ����� �������

void copy_to_lcd_buffer(char* src)
{
	for (int i=0; i<sizeof(lcd8544_buff); i++)
	{
		lcd8544_buff[i]=src[i];
	}
}

void full_screen(char value)
{
	for (int i=0; i<sizeof(lcd8544_buff); i++)
		{
			lcd8544_buff[i]=value;
		}
}

void clear_screen(void)
{
	full_screen((char)0x00);
}



void EXTI9_5_IRQHandler()//��������� ������ ������ ����������
	{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	  xHigherPriorityTaskWoken = pdFALSE;
	static volatile tsystem_event_type sys_event_tmp;
	EXTI_ClearFlag(EXTI_Line7);
	sys_event_tmp.event_source = BLUETOOTH;
	xQueueSendToBackFromISR(xEventQueue, &sys_event_tmp, &xHigherPriorityTaskWoken);

	}


tfire_mode test_fire_mode_switch(void){

	int ref_adc_val =  ADC_values[2];//ADC_GetConversionValue(ADC3);
	if (ref_adc_val < 1000)
	{
		return SINGLE;
	}
	else if (ref_adc_val > 3000)
	{
		return AUTO;
	}
	else
	{
		return SEMI_AUTO;
	}
}



void lock_firmware(void)
{
#ifdef NDEBUG
    if (FLASH_GetReadOutProtectionStatus() == RESET)
    {
        FLASH_Unlock();
        FLASH_ReadOutProtection(ENABLE);
        FLASH_Lock();
    }
#endif
}


void bt_hit_processing(trx_packet new_rx_bt_packet)//������������ ���������, �������� �� ������
{
	if(game_status==OUT_OF_GAME)return;//����� ��� ����, ���������� �����
	if((armadaSystem.player.team_color==new_rx_bt_packet.team_id)&&(armadaSystem.friendly_fire==false)) return;//����� �� ������ ����� �� �������, � ������������� ����� ������� - ���������� �����
	if(safe_counter==0) //���� ��� ��������������� ���������
				{
					safe_counter=SAFE_TIME;
					if(armadaSystem.player.health > new_rx_bt_packet.damage)//���� ������, ��� ������� ��������
					{
						armadaSystem.player.health =  armadaSystem.player.health - new_rx_bt_packet.damage;
						armadaSystem.wav_player.type_of_sound_to_play = HIT;//���� ������������� ���� �������
						xSemaphoreGive(xWavPlayerManagerSemaphore);
					}
					else {
						armadaSystem.player.health = 0;
						game_status = OUT_OF_GAME;
						armadaSystem.wav_player.type_of_sound_to_play = GAME_OVER;//���� ������������� ���� "���� ���������"
						if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)
						{

#ifndef 	COLOR_LCD

#else


							if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
							{
								drawBMPfromFlashUseDMA(game_over_pic,0,0,128,128,Rotation_0);

								xSemaphoreGive(xColorLCDSemaphore);
							}
#endif




						}
						xSemaphoreGive(xWavPlayerManagerSemaphore);
					}

if(game_status != OUT_OF_GAME)
{
#ifndef 	COLOR_LCD
	  lcd_health_update();
#else
	  color_lcd_health_update();
#endif
}

				}
#ifdef SENSORS_BECKLIGHT
	set_sensor_color(index_to_color[new_rx_bt_packet.team_id],NUMBER_OF_SENSORS_FRAMES,zone_1);
	set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_1);
	set_sensor_color(index_to_color[new_rx_bt_packet.team_id],NUMBER_OF_SENSORS_FRAMES,zone_2);
	set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_2);
	set_sensor_color(index_to_color[new_rx_bt_packet.team_id],NUMBER_OF_SENSORS_FRAMES,zone_3);
	set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_3);
	set_sensor_color(index_to_color[new_rx_bt_packet.team_id],NUMBER_OF_SENSORS_FRAMES,zone_4);
	set_sensor_vibro(NUMBER_OF_SENSORS_FRAMES,zone_4);

#else
	sensors[zone_1]=index_to_color[new_rx_bt_packet.team_id];
	sensors[zone_2]=index_to_color[new_rx_bt_packet.team_id];
	sensors[zone_3]=index_to_color[new_rx_bt_packet.team_id];
	sensors[zone_4]=index_to_color[new_rx_bt_packet.team_id];
#endif
//�������������� ���������� xLastWakeTime ������� ��������.
//	xLastWakeTime = xTaskGetTickCount ();

	vTaskDelay(TIC_FQR);//�� ������� ������� ��������� ����
//			vTaskDelayUntil( &xLastWakeTime, TIC_FQR );
//			sensors[zone]=index_to_color[rx_pack.team_id];
//			vTaskDelay(TIC_FQR/2);

#ifdef SENSORS_BECKLIGHT
	set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone_1);
	set_sensor_vibro(0,zone_1);
	set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone_2);
	set_sensor_vibro(0,zone_2);
	set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone_3);
	set_sensor_vibro(0,zone_3);
	set_sensor_color(index_to_color[armadaSystem.player.team_color],armadaSystem.backlight_level,zone_4);
	set_sensor_vibro(0,zone_4);

#else
	sensors[zone_1]=BLACK;
	sensors[zone_2]=BLACK;
	sensors[zone_3]=BLACK;
	sensors[zone_4]=BLACK;
#endif
}



void BKPinit()
{
    /* Enable PWR and BKP clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_BKP, ENABLE);
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

}


void RTC_Init(){

    	 volatile  int i;
    	  i++;
    	  i=90;
/*
    	  NVIC_InitTypeDef NVIC_InitStructure;

    	     // Configure one bit for preemption priority
    	     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    	     // Enable the RTC Interrupt
    	     NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    	     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    	     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    	     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    	     NVIC_Init(&NVIC_InitStructure);
*/



    RCC_LSEConfig(RCC_LSE_ON);

      while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET){

      }

      RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
      RCC_RTCCLKCmd(ENABLE);

      RTC_WaitForSynchro();
      RTC_WaitForLastTask();
      RTC_ITConfig(RTC_IT_SEC, ENABLE);
      RTC_WaitForLastTask();
      RTC_SetPrescaler(32767); // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
      RTC_WaitForLastTask();


}
/*

*/

void RTC_IRQHandler(void)
{
   if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
   {
      RTC_ClearITPendingBit(RTC_IT_SEC);
      RTC_WaitForLastTask();
   }

//   CounterToFtime ((uint32_t)RTC_GetCounter(),&date);
//   USART1_SendStr(int_to_str(date.second,2));
//   USART1_SendStr("\n\r");
//   lcd_time_update(date.hour, date.minute, date.second);

   //times=RTC_GetCounter();
}


// ������� �������������� ������������� ���� � ������� � �������� ��������
uint32_t FtimeToCounter(ftime_t * ftime)
{
uint8_t a;
uint16_t y;
uint8_t m;
uint32_t JDN;

// ���������� ����������� �������������
a=(14-ftime->month)/12;
y=ftime->year+4800-a;
m=ftime->month+(12*a)-3;
// ��������� �������� �������� ���������� ���
JDN=ftime->day;
JDN+=(153*m+2)/5;
JDN+=365*y;
JDN+=y/4;
JDN+=-y/100;
JDN+=y/400;
JDN+=-32045;
JDN+=-JD0; // ��� ��� ������� � ��� �����������, ������ ��� ������� ������ �� 01 ��� 2001
JDN*=86400;     // ��������� ��� � �������
JDN+=(ftime->hour*3600); // � ��������� ��� �������� �������� ���
JDN+=(ftime->minute*60);
JDN+=(ftime->second);
// ����� ����� ���������� ������ � 00-00 01 ��� 2001
return JDN;
}

// ������� �������������� �������� �������� � ������������� ���� � �����
void CounterToFtime(uint32_t counter,ftime_t * ftime)
{
uint32_t ace;
uint8_t b;
uint8_t d;
uint8_t m;

ace=(counter/86400)+32044+JD0;
b=(4*ace+3)/146097; // ����� �� ��������� ������ �������� ��-�� ������������ 4*ace ??
ace=ace-((146097*b)/4);
d=(4*ace+3)/1461;
ace=ace-((1461*d)/4);
m=(5*ace+2)/153;
ftime->day=ace-((153*m+2)/5)+1;
ftime->month=m+3-(12*(m/10));
ftime->year=100*b+d-4800+(m/10);
ftime->hour=(counter/3600)%24;
ftime->minute=(counter/60)%60;
ftime->second=(counter%60);
}


#define item_height 8
#define item_symbolWidth 6

const unsigned char item_pos [][2] =
    {
	{0,item_height},
	{0,item_height*2},
	{0,item_height*3},
	{0,item_height*4},
	{0,item_height*5},
	{item_symbolWidth*7,item_height*5}
    };

void go_to_next_menu_item (uint8_t* item){
	fillRect(item_pos[*item][0],item_pos[*item][1],item_symbolWidth,item_height,0);//������� ������ � ������� �������
	if (*item!=5)
	{*item = (*item)+1;}
	else{
		*item = 0;
	}
	lcd8544_putstr(item_pos[*item][0],item_pos[*item][1],">", 0);
	if (!screen_auto_refresh)lcd8544_dma_refresh();

}


/***********************
 *
 *
 ************************/
void getDateClockSettings (void){

	uint8_t curr_item = 0;
	bool apply = false;
	clear_screen();
	lcd8544_putstr(0, 0, "*Date & clock*", 0); // ����� ���������
	lcd8544_putstr(0, item_height,   "> month:   00", 0); // ����� ������ ������
	lcd8544_putstr(0, item_height*2, "  day:     00", 0); // ����� ������ ������
	lcd8544_putstr(0, item_height*3, "  hour:    00", 0); // ����� ������� ������
	lcd8544_putstr(0, item_height*4, "  minutes: 00", 0); // ����� ������ ������
	lcd8544_putstr(0, item_height*5, " APPLY  CANSEL", 0); // ����� ������ ������
	if (!screen_auto_refresh) lcd8544_dma_refresh(); // ����� ������ �� ����� ! ��� ����� ������ ����� �� ����� !
	xSemaphoreTake( xKeysSemaphore, 30 );//������� �������
	keyboard_event = no_key_pressing;
	reload_key_event = no_key_pressing;
	while (! apply ){
	xSemaphoreTake( xKeysSemaphore, portMAX_DELAY );
	switch(reload_key_event){
  			case no_key_pressing:
  			{

  			}
  			break;
  			case key_pressing:
  			{
  				go_to_next_menu_item((uint8_t*)&curr_item);
  				reload_key_event = no_key_pressing;
  			}
  			break;
  	 		default:
  	 		{
  	 			reload_key_event = no_key_pressing;
  	   		}
  	 		break;
		}


	}
	clear_screen();
}



void send_message_by_bluetooth(tir_message message){
	 uint8_t mask = (1<<7);
	  USART1_FlushTxBuf(); //�������� ���������� �����
	  USART1_SendStr("h");
	  while (mask)
	  {
		  if ((message.ID)&(mask)) USART1_SendStr("1");
		  else USART1_SendStr("0");
		  mask = mask >> 1;
	  }
	  mask = (1<<7);
	  while (mask)
	 	  {
	 		  if ((message.param)&(mask)) USART1_SendStr("1");
	 		  else USART1_SendStr("0");
	 		  mask = mask >> 1;
	 	  }
	  mask = (1<<7);
	  while (mask)
	 	 	  {
	 	 		  if (((uint8_t)0xE8)&(mask)) USART1_SendStr("1");
	 	 		  else USART1_SendStr("0");
	 	 		  mask = mask >> 1;
	 	 	  }

	  if(message.ID == SYSTEM_DATA){
		  switch (message.param){
		  case  CLONING_DATA:
		  {


		  }
		  break;
		  case TAG_INIT_DATA:
		  {
			  mask = (1<<7);
			  while (mask){
				if (((uint8_t)(message.clone_data_union.tag_init_data.player_id))&(mask))  USART1_SendStr("1");
				else USART1_SendStr("0");
		 	 	  mask = mask >> 1;
			  }
			  mask = (1<<7);
			  while (mask){
			 	if (((uint8_t)(message.clone_data_union.tag_init_data.team_id))&(mask))  USART1_SendStr("1");
			 	else USART1_SendStr("0");
			 	mask = mask >> 1;
			  }
			  mask = (1<<7);
			  while (mask){
			 	if (((uint8_t)(message.clone_data_union.tag_init_data.game_status))&(mask))  USART1_SendStr("1");
			 	else USART1_SendStr("0");
			 	mask = mask >> 1;
			  }
			  mask = (1<<7);
			  while (mask){
			 	if (((uint8_t)(message.clone_data_union.tag_init_data.health))&(mask))  USART1_SendStr("1");
			 	else USART1_SendStr("0");
			 	mask = mask >> 1;
			  }
			  mask = (1<<7);
			  while (mask){
			 	if (((uint8_t)(message.clone_data_union.tag_init_data.command))&(mask))  USART1_SendStr("1");
			 	else USART1_SendStr("0");
			 	mask = mask >> 1;
			  }

			  uint8_t* p_tmp;
			  p_tmp = (uint8_t*)&message.clone_data_union.tag_init_data.game_session_ID;
			  for(int i=0;i<4;i++){//�������� 4 ����� ������
				  mask = (1<<7);
				  while (mask){
			  					if ((*p_tmp)&(mask))  USART1_SendStr("1");
			  		 	 		  else USART1_SendStr("0");
			  				 	mask = mask >> 1;
			  				  }
				  p_tmp++;
			  	}//[for]

			  mask = (1<<7);
			  while (mask){
			 	if (((uint8_t)(message.clone_data_union.tag_init_data.Checksum))&(mask))  USART1_SendStr("1");
			 	else USART1_SendStr("0");
			 	mask = mask >> 1;
			  }


		  }
		  break;
		  default: break;

		  }//[ switch (message.param)]

	  }//[if(message.ID == System_Data)]
	  USART1_SendStr("t");
}



uint8_t checksum_for_clone_data(uint8_t* clone_data){
uint8_t checksum;
checksum = 0;
for (int i=0; i<35; i++)
{
	checksum+=clone_data[i];
}
return checksum;
}

uint8_t checksum_for_tag_init_data(ttag_init_data tag_init_data){
	volatile uint8_t checksum;
	checksum = 0;
	checksum +=(uint8_t)tag_init_data.player_id;
	checksum +=(uint8_t)tag_init_data.team_id;
	checksum +=(uint8_t)tag_init_data.game_status;
	checksum +=(uint8_t)tag_init_data.health;
	checksum +=(uint8_t)tag_init_data.command;
	uint8_t* p_tmp;
	p_tmp = (uint8_t*)&tag_init_data.game_session_ID;


	for (int i=0; i<4; i++)
	{
		checksum+=*p_tmp;
		p_tmp++;

	}
	return checksum;

}




void initGUI(void){//����� �� ����� ��������� ����������������� ����������
#ifndef 	COLOR_LCD
	  LCD_BL_OFF;
	  clear_screen();
	  lcd_health_update();

//	  drawDigit(52+2,0,10,1);
	  drawBitmap(84-16,0, battary_img,16,8,1);
	  drawSmallDigit(84-16-1,8-1,0,1);
	  drawSmallDigit(84-16+4-1,8,10,1);
	  drawSmallDigit(84-16+8-1,8-1,0,1);
	  drawSmallDigit(84-16+12,8-1,0,1);
	  drawBitmap(0,16,(char*)&clip_img[6][0],16,24,1);
	  drawDigit(16,16+4,3,1);
	  drawDigit(28,16+4,5,1);

	  drawBitmap(0,0,heart[0],16,16,1);
	  //drawDigit(40,0,0,1);
	  drawSmallDigit(0,41,1,1);
	  drawSmallDigit(5,41,5,1);
//	fillRect(84-6*4 -1,48-8-1,6*4+1,8+1,1);//������� ����������� ��������-���� � ���������
/*
	  lcd8544_putstr(84-6*4-2,48-8 ,"SAFE", 1); // ����� ������ ������
	  drawFastHLine(84-6*4-3,48-1,6*4+2,1);
	  drawFastVLine(84-1,48-8-1,9,1);
	  drawFastVLine(84-2,48-8-1,9,1);
      if (!screen_auto_refresh) lcd8544_dma_refresh();
*/
  //    armadaSystem.gun.rounds=0;
	  lcd_rounds_update();
	  lcd_clips_update();
/*	  vTaskDelay(600);
	  armadaSystem.gun.rounds=105;
	  lcd_rounds_update();
	  vTaskDelay(600);
	  armadaSystem.gun.rounds=35;
	  lcd_rounds_update();
*/

	lcd_fire_mode_update(test_fire_mode_switch());


	  if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//���� ���������� �����������
	        {
		  	  drawDigit(52+2,0,10,1);//������ �������� ������

	        }
	    else//���� ���������� ���������
	        {
	    	fillRect(52+2,0,12,16,0);//������� �������� ������
	        }

	  if (!screen_auto_refresh)lcd8544_dma_refresh();

#else
	  if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)

{
	  ClrScrn();
	drawBMPfromFlashUseDMA(bullet,64,40,64,21,Rotation_90);
	drawBMPfromFlashUseDMA(clip_2,93,40,35,64,Rotation_0);

	xSemaphoreGive(xColorLCDSemaphore);
}


//	drawBMPfromFlashUseDMA(bluetooth1,0,0,16,32,Rotation_0);



    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//���� ���������� �����������
        {

    		 if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
    		{
    		drawBMPfromFlash(bluetooth1,16,32);
    		xSemaphoreGive(xColorLCDSemaphore);
    		}
        }
    else//���� ���������� ���������
        {
    	 if(xSemaphoreTake(xColorLCDSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//���� LCD �����, ���� 2 �
    	  {

    		 FillRect(iBlack,0,0,16,32);
    		 xSemaphoreGive(xColorLCDSemaphore);
    	   }

        }




	color_lcd_health_update();
	color_lcd_rounds_update();
	color_lcd_clips_update();
	taskENTER_CRITICAL();
	color_lcd_fire_mode_update(curr_fire_mode);
	taskEXIT_CRITICAL();



#endif
}





#ifdef SENSORS_BECKLIGHT
void set_sensor_color(uint8_t color, uint8_t brightness_level, TDamageZone zone){
//��������� ������
if (brightness_level > NUMBER_OF_SENSORS_FRAMES)brightness_level=NUMBER_OF_SENSORS_FRAMES;
for(uint8_t i=0; i<NUMBER_OF_SENSORS_FRAMES; i++ ){
	sensors[i][zone] = BLACK;
}
if(color==BLACK)return;
switch ((uint8_t)color){
if(brightness_level>NUMBER_OF_SENSORS_FRAMES)brightness_level=NUMBER_OF_SENSORS_FRAMES;
case RED:
case RED|HIGH_BRIDHTNESS:
case BLUE :
case BLUE|HIGH_BRIDHTNESS :
case GREEN:
case GREEN|HIGH_BRIDHTNESS :
case YELLOW|HIGH_BRIDHTNESS:
	{
		volatile bool skip = false;
//��������� ������ ������ �������� ������ �������
		for(uint8_t i=0; i<brightness_level; i++){
			if (!skip){sensors[i][zone] = color;}
			else
			{
				i--;
				sensors[NUMBER_OF_SENSORS_FRAMES/2+i][zone] = color;
			}
			skip = !skip;
		}

	}
	break;
case CYAN:
	{

	}
	break;

case YELLOW:
	{
		volatile bool swap = false;
		for(uint8_t i=0; i<(brightness_level);i+=2){
			if(!swap){
				sensors[i][zone] = RED;
				sensors[NUMBER_OF_SENSORS_FRAMES/2+i][zone] = GREEN;
			}
			else{
				sensors[i][zone] = GREEN;
				sensors[NUMBER_OF_SENSORS_FRAMES/2+i][zone] = RED;
			}
			swap = !swap;
		}

	}
	break;
case MAGENTA:
	{

	}
	break;
case WHITE:
	{

	}
	break;
case BLACK:
	{

	}
	break;
default: break;
}
}



void set_sensor_vibro(uint8_t power_level, TDamageZone zone){
	for(uint8_t i=0; i<NUMBER_OF_SENSORS_FRAMES; i++ ){
		sensors[i][zone] &= ~VIBRO_HP;
	}
	if(!power_level)return;
	volatile bool skip = false;
	//��������� ������ ������ �������� ������ �������
			for(uint8_t i=0; i<power_level; i++){
				if (!skip){sensors[i][zone] |= VIBRO_HP;}
				else
				{
					i--;
					sensors[NUMBER_OF_SENSORS_FRAMES/2+i][zone] |=VIBRO_HP ;
				}
				skip = !skip;
			}

}
#endif

/*
void bt_tag_init(){
	tir_message tag_init_message;
	tag_init_message.ID = SYSTEM_DATA ;
	tag_init_message.param = TAG_INIT_DATA;
	tag_init_message.control_byte = Valid_value;
	tag_init_message.clone_data_union.tag_init_data.player_id = armadaSystem.player.id;
	tag_init_message.clone_data_union.tag_init_data.team_id = armadaSystem.player.team_color;
	tag_init_message.clone_data_union.tag_init_data.game_status = game_status;
	tag_init_message.clone_data_union.tag_init_data.health = armadaSystem.player.health;
	tag_init_message.clone_data_union.tag_init_data.game_session_ID = game_session_ID;
	tag_init_message.clone_data_union.tag_init_data.Checksum =  checksum_for_tag_init_data(tag_init_message.clone_data_union.tag_init_data);
	send_message_by_bluetooth(tag_init_message);
}
*/

#if DEVICE_ROLE==BANDANA
void bt_tag_init_with_game_status(bool status, ttag_init_command command){
	volatile tir_message tag_init_message;
	tag_init_message.ID = SYSTEM_DATA ;
	tag_init_message.param = TAG_INIT_DATA;
	tag_init_message.control_byte = Valid_value;
	tag_init_message.clone_data_union.tag_init_data.player_id = armadaSystem.player.id;
	tag_init_message.clone_data_union.tag_init_data.team_id = armadaSystem.player.team_color;
	tag_init_message.clone_data_union.tag_init_data.game_status = status;
	tag_init_message.clone_data_union.tag_init_data.health = armadaSystem.player.health;
	tag_init_message.clone_data_union.tag_init_data.command = (uint8_t)command;
	tag_init_message.clone_data_union.tag_init_data.game_session_ID = bandana_game_session_ID;
	tag_init_message.clone_data_union.tag_init_data.Checksum =  checksum_for_tag_init_data(tag_init_message.clone_data_union.tag_init_data);
	send_message_by_bluetooth(tag_init_message);
}
#endif

