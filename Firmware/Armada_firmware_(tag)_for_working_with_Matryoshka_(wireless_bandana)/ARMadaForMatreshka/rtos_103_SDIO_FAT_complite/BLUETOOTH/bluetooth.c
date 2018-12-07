//#include "bluetooth.h"

//#include <stdio.h>
//#include <string.h>
//#include "types.h"

#include "bluetooth.h"
#include "global_variables.h"
#include <ILI9163.h>

extern const unsigned char team_color_to_symbol[4];


volatile uint8_t bt_rx_buffer[RX_BLUETOOTH_BUFFER_SIZE]; 	//Буффер для ИК-пакета, принятого по блютус
volatile bool bt_header_received=false;
volatile uint16_t bit_in_bt_rx_buff=0; 			//Количество бит, принятых по блютус
volatile trx_packet bt_rx_packet;



extern const uint8_t damage_value [];

void set_bt_buffer_bit(uint8_t index, bool value){	//Задаем значение биту в буфере ИК-приемника
uint8_t byte_index;
uint8_t bit_index;
byte_index = index/8; //Определяем, в каком байте нахадится нужный бит
bit_index = index - (byte_index*8);//Определяем номер бита в байте
if(value)
		{
			bt_rx_buffer[byte_index] |= (1<<(7-bit_index));
		}
else	{
			bt_rx_buffer[byte_index] &= ~(1<<(7-bit_index));
		}
}



/*****************************************
* Извлекаем данние из пакета, принятого по блютус
******************************************/

trx_event parsing_bt_data(void) //анализируем пакет, полученный по блютус, извекаем из него данные
{
	volatile unsigned char tmp_char;
	trx_event result;

	result = NOT_EVENT;

	while ( USART1_GetRxCount())//пока не считаем все символы из буфера
	{
		tmp_char = USART1_GetChar();
		switch(tmp_char)
		{
			case 'L':
			{
				return BANDANA_BATTARY_LOW;
			}

			case 'f':
			{

				return CHITER_DETECTED;
			}

			case 'h'://нашли заголовок пакета
			{
				bt_header_received = true; //фиксируем получение заголовка
				bit_in_bt_rx_buff=0;//ставим курсор в начало буфера пакета
			}
			break;
			case '0'://получен бит "0"
			{
				if(bt_header_received) set_bt_buffer_bit(bit_in_bt_rx_buff++, false);//если заголовок получен, заносим значение бита в буфер пакета
			}
			break;
			case '1'://получен бит "1"
			{
				if(bt_header_received) set_bt_buffer_bit(bit_in_bt_rx_buff++, true);//если заголовок получен, заносим значение бита в буфер пакета
			}
			break;
			case 'e'://пакет битый
			{
				bt_header_received = false;//нужно искать следующий заголовок
				return RX_ERROR;//выходим с ошибкой
			}
			break;
			case 't'://таймаут приема бита
			{

				if((bt_header_received)&&(bit_in_bt_rx_buff>0))//если заголовок получен и буфер пакета не пустой
				{



					switch(bit_in_bt_rx_buff)//проверим, сколько бит принято
					{
						case 14:
						{
							result = RX_COMPLETE;			//Генерим событие "принят пакет"
							break;
						}
						case 24:
#if DEVICE_ROLE==TAG
						case 8*13://это сообщение с данными для инициализации тага
#endif
						case 8*39://это сообщение с данными для клонирования
						{
							result	= RX_MESSAGE_COMPLETE;//принято сообщение;
							break;
						}
						default:
						{
							result = RX_ERROR;			//генерируем событие - "ошибка приёма"
						}
					}

						bt_header_received = false;//заголовок обработали
						return result;
				//		return RX_COMPLETE;//выходим с сообщением о получении пакета
				}
				else
				{
					bt_header_received = false;//заголовок обработали
				}
			}
			break;

		}



	}

//всё считали, собйтий нет
return result;

}

bool get_bt_buffer_bit(uint8_t index){		//Считываем значение бита в буфере ИК-приемника
uint8_t byte_index;
uint8_t bit_index;
byte_index = index/8; //Определяем, в каком байте нахадится нужный бит
bit_index = index - (byte_index*8);//Определяем номер бита в байте
if(bt_rx_buffer[byte_index]&(1<<(7-bit_index))) return true;
else return false;
}



void test_bt_data()
{

		switch(parsing_bt_data())//проверим роиемный буфер
		{

			case CHITER_DETECTED:
			{

				if((bt_bandana_already_been_on)&&(game_status==IN_GAME)){
					if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2) )== pdTRUE)
					{
						chiter_detected=true;
#ifdef DISPLAY_ENABLE
						display_show_chiter_picter();
#endif
						armadaSystem.wav_player.type_of_sound_to_play = USER_SOUND;
						memcpy((void*)armadaSystem.wav_player.message_sound_file_name,"/syssnd/chiter.wav",strlen("/syssnd/chiter.wav"));
						armadaSystem.wav_player.message_sound_file_name[strlen("/syssnd/chiter.wav")] = 0;
						xSemaphoreGive(xWavPlayerManagerSemaphore);
						game_status=OUT_OF_GAME;

					}
				}


			}
			break;

			case BANDANA_BATTARY_LOW:{
				armadaSystem.wav_player.type_of_sound_to_play = USER_SOUND;
				memcpy((void*)armadaSystem.wav_player.message_sound_file_name,"/syssnd/banbtlow.wav",strlen("/syssnd/banbtlow.wav"));
				armadaSystem.wav_player.message_sound_file_name[strlen("/syssnd/banbtlow.wav")] = 0;
				 xSemaphoreGive(xWavPlayerManagerSemaphore);

			}
			break;

			case RX_COMPLETE: 	//получен пакет
			{
				if(!get_bt_buffer_bit(0)) //если этот бит равен 0, то это пакет с данными (выстрел)
								{
									bt_rx_packet = get_bt_packet_value();
									bt_hit_processing(bt_rx_packet);
									USART1_FlushRxBuf();
									bt_header_received=false;
								}

			}
			break;
			case RX_ERROR:		//ошибка приема
			{

			}
			break;
			case RX_MESSAGE_COMPLETE://принято сообщение
			{
				static volatile tir_message bt_message;
				bt_message = get_bt_message_value();

				switch(bt_message.ID)//если имя команды
				{

					case SYSTEM_DATA://приняты системные данные
					{

#if DEVICE_ROLE == TAG
						switch(bt_message.param)//выясним, какие это данные
						{
							case CLONING_DATA://
							{

							}
							break;
							case TAG_INIT_DATA:
							{


								switch((ttag_init_command)bt_message.clone_data_union.tag_init_data.command)
								{
									case NEW_CONNECT:
									{
										static volatile bool connection_has_been = false;
										//если это НЕ первое соединение после включения тага и игровая сессия не менялась
										if ((armadaSystem.game_session_ID == bt_message.clone_data_union.tag_init_data.game_session_ID)&&(connection_has_been)) {
											if (game_status!=bt_message.clone_data_union.tag_init_data.game_status)
											{
												armadaSystem.player.team_color=bt_message.clone_data_union.tag_init_data.team_id;
												set_team_color(armadaSystem.player.team_color);
												game_status=bt_message.clone_data_union.tag_init_data.game_status;
												if(game_status)
												{
#ifdef DISPLAY_ENABLE
													display_init_gui();
#endif
													xSemaphoreGive(xGameOverSemaphore);//отдаем семафор
												}
												else
												{
#ifdef DISPLAY_ENABLE
													display_show_game_over_picture();
#endif
												}

											}
										}
										//если это ПЕРВОЕ соединение после включения тага и игровая сессия не менялась
										else if ((armadaSystem.game_session_ID == bt_message.clone_data_union.tag_init_data.game_session_ID)&&(!connection_has_been))
										{
											connection_has_been = true;
											armadaSystem.player.team_color=bt_message.clone_data_union.tag_init_data.team_id;
											set_team_color(armadaSystem.player.team_color);
											armadaSystem.gun.rounds = 0;
											armadaSystem.player.health = bt_message.clone_data_union.tag_init_data.health;
#ifdef DISPLAY_ENABLE
											display_update_rounds();
											display_update_clips();
											display_update_health();
#endif
											if (game_status!=bt_message.clone_data_union.tag_init_data.game_status)
											{//если новый игровой статус
												if(bt_message.clone_data_union.tag_init_data.game_status)
												{//если новый статус - "В ИГРЕ"
#ifdef DISPLAY_ENABLE
													display_init_gui();
#endif
													xSemaphoreGive(xGameOverSemaphore);//отдаем семафор
												}
												else
												{//новый игровой статус - "ВНЕ ИГРЫ"

													if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)
													{
#ifdef DISPLAY_ENABLE
														display_show_game_over_picture();
#endif
													}
												}

												game_status=bt_message.clone_data_union.tag_init_data.game_status;


											}
											else//игровой статус не поменялся
											{
												if(game_status){//если в игре

//													initGUI();
//													xSemaphoreGive(xGameOverSemaphore);//отдаем семафор

												}
												else//если вне игры (и это первое соединение после включения)
												{

#ifdef DISPLAY_ENABLE
													display_show_game_over_picture();
#ifdef COLOR_LCD
														 if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//если соединение установлено
												        {

															 display_show_bluetooth_icon();

														}//[ if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//если соединение установлено]
#endif
#endif
												}

											}//[else//игровой статус не поменялся]
										}
										else if(armadaSystem.game_session_ID != bt_message.clone_data_union.tag_init_data.game_session_ID)
										{//новая игровая сессия
											armadaSystem.game_session_ID = bt_message.clone_data_union.tag_init_data.game_session_ID;
											armadaSystem.player.team_color=bt_message.clone_data_union.tag_init_data.team_id;
											set_team_color(armadaSystem.player.team_color);
											armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
//											stopWavPlayer();
											armadaSystem.wav_player.type_of_sound_to_play = NOTHING;
											xSemaphoreGive(xWavPlayerManagerSemaphore);
											if(xSemaphoreTake(xSDcardLockSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если SD карта занята, ждем 2 с
											{
												save_parameters_to_sd_card();
												xSemaphoreGive(xSDcardLockSemaphore);
											}
											armadaSystem.gun.rounds=0;
											armadaSystem.player.health=bt_message.clone_data_union.tag_init_data.health;


											if(game_status!=bt_message.clone_data_union.tag_init_data.game_status)
											{//если новый игровой статус
												if(bt_message.clone_data_union.tag_init_data.game_status)
												{//если новый статус - "В ИГРЕ"
												//	initGUI();

#ifdef DISPLAY_ENABLE
													display_init_gui();
#endif
													xSemaphoreGive(xGameOverSemaphore);//отдаем семафор

												}
												else
												{//новый игровой статус - "ВНЕ ИГРЫ"

												}

												game_status=bt_message.clone_data_union.tag_init_data.game_status;
											}
											else//игровой статус не поменялся
											{

												if (game_status)//если в игре
												{
#ifdef DISPLAY_ENABLE
													display_update_rounds();
													display_update_clips();
													display_update_health();
#endif
												}
												else//если вне игры
												{
#ifdef DISPLAY_ENABLE
													display_show_game_over_picture();

													 if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//если соединение установлено
											        {
														 display_show_bluetooth_icon();

													}//[ if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//если соединение установлено]
#endif

												}

											}

										}

										connection_has_been = true;
									}
									break;
									case NEW_GAME:
									{
										chiter_detected=false;
										armadaSystem.game_session_ID = bt_message.clone_data_union.tag_init_data.game_session_ID;
										armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
										armadaSystem.player.team_color=bt_message.clone_data_union.tag_init_data.team_id;
										set_team_color(armadaSystem.player.team_color);
										armadaSystem.wav_player.type_of_sound_to_play = NOTHING;
										xSemaphoreGive(xWavPlayerManagerSemaphore);
										if(xSemaphoreTake(xSDcardLockSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)//если SD карта занята, ждем 2 с
										{
											save_parameters_to_sd_card();
											xSemaphoreGive(xSDcardLockSemaphore);
										}
										armadaSystem.gun.rounds=0;
										armadaSystem.player.health=bt_message.clone_data_union.tag_init_data.health;

										//initGUI();
#ifdef DISPLAY_ENABLE
										display_init_gui();
#endif
										xSemaphoreGive(xGameOverSemaphore);//отдаем семафор
										game_status = true;




									}
									break;
									case STOP_GAME:
									{
										game_status = OUT_OF_GAME;

#ifdef DISPLAY_ENABLE

										if(xSemaphoreTake(xGameOverSemaphore, (portTickType)(TIC_FQR*2)/*600*/ )== pdTRUE)
										{
											display_show_game_over_picture();
										}
										if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//если соединение установлено
								        {
											 display_show_bluetooth_icon();//рисуем эмблемку блютус

										}//[ if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == 1)//если соединение установлено]

#endif



									}
									break;
									case HEALTH_UPDATE:
									{
										armadaSystem.player.health=bt_message.clone_data_union.tag_init_data.health;
#ifdef DISPLAY_ENABLE
										display_update_health();
#endif
/*
#ifndef 	COLOR_LCD
										  lcd_health_update();
#else
										  color_lcd_health_update();
#endif
*/
									}
									break;
									case PLAYER_TEAM_ID_UPDATE:
									{
										set_player_id(bt_message.clone_data_union.tag_init_data.player_id);
										set_team_color((tteam_color)bt_message.clone_data_union.tag_init_data.team_id);


										if(game_status!=bt_message.clone_data_union.tag_init_data.game_status)
										{
											game_status=bt_message.clone_data_union.tag_init_data.game_status;
										}
										if (game_status)//если в игре
										{

										}
										else//если вне игры
										{

										}


									}
									break;
									default:
									{
									}
									break;


								}//[switch(bt_message.clone_data_union.tag_init_data.command)]


/*
								if(bt_message.clone_data_union.tag_init_data.Checksum == checksum_for_tag_init_data(bt_message.clone_data_union.tag_init_data))
								{

									game_status = bt_message.clone_data_union.tag_init_data.game_status;
									set_player_id(bt_message.clone_data_union.tag_init_data.player_id);
									set_team_color((tteam_color)bt_message.clone_data_union.tag_init_data.team_id);

									if (armadaSystem.player.health != bt_message.clone_data_union.tag_init_data.health)
									{
										armadaSystem.player.health = bt_message.clone_data_union.tag_init_data.health;
										if(game_status != OUT_OF_GAME)
										{
#ifndef 	COLOR_LCD
										  lcd_health_update();
#else
										  color_lcd_health_update();
#endif
										}
										else //вне игры
										{
#ifndef 	COLOR_LCD
										  lcd_health_update();
#else
										  color_lcd_health_update();
#endif
										}
									}
									if (bt_message.clone_data_union.tag_init_data.game_session_ID != game_session_ID)
									{
										game_session_ID = bt_message.clone_data_union.tag_init_data.game_session_ID;
										armadaSystem.gun.rounds = 0;
										armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;
#ifndef 	COLOR_LCD
										lcd_rounds_update();
										lcd_clips_update();
//	lcd_fire_mode_update(curr_fire_mode);
#else
										color_lcd_rounds_update();
										color_lcd_clips_update();
//	color_lcd_fire_mode_update(curr_fire_mode);
#endif
									}



								}
*/
							}
							break;

						}

						break;
#endif
					}

					break;




					case Add_Health: //добавить "жизни"
				    {
				    	//код для добавления жизни
					    break;
				    }
					case Add_Rounds://добавить "патронов"
					{
						//код для добавления патронов
				        break;
				    }
				    case Change_color:
					{

#if DEVICE_ROLE==TAG
						//код для смены цвета
						if((bt_message.param>=0)&&(bt_message.param<=3))
						{
							armadaSystem.player.team_color=bt_message.param;
							set_team_color(armadaSystem.player.team_color);
							armadaSystem.wav_player.type_of_sound_to_play = MESSAGE;
							switch(armadaSystem.player.team_color)
							{
								case Red:
								{
									memcpy((void*)armadaSystem.wav_player.message_sound_file_name,"nowred.wav",strlen("nowred.wav"));
									armadaSystem.wav_player.message_sound_file_name[strlen("nowred.wav")] = 0;
									//armadaSystem.wav_player.message_sound_file_name = ;
								}
								break;
								case Blue:
								{
									memcpy((void*)armadaSystem.wav_player.message_sound_file_name,"nowblue.wav",strlen("nowblue.wav"));
									armadaSystem.wav_player.message_sound_file_name[strlen("nowblue.wav")] = 0;
									//armadaSystem.wav_player.message_sound_file_name = "nowblue.wav";
								}
								break;
								case Yellow:
								{
									memcpy((void*)armadaSystem.wav_player.message_sound_file_name,"nowyell.wav",strlen("nowyell.wav"));
									armadaSystem.wav_player.message_sound_file_name[strlen("nowyell.wav")] = 0;
									//armadaSystem.wav_player.message_sound_file_name = "nowyellow.wav";
								}
								break;
								case Green:
								{
									memcpy((void*)armadaSystem.wav_player.message_sound_file_name,"nowgreen.wav",strlen("nowgreen.wav"));
									armadaSystem.wav_player.message_sound_file_name[strlen("nowgreen.wav")] = 0;
									//armadaSystem.wav_player.message_sound_file_name = "nowgreen.wav";
								}
								break;
								default:
								{

								}
								break;

							}

							xSemaphoreGive(xWavPlayerManagerSemaphore);
							USART1_PutChar(team_color_to_symbol[armadaSystem.player.team_color]);



						}
#endif
						break;
					}
				    case Command://какая то дополнительноя команда
				    {
				    	switch(bt_message.param)//выясним, какая это командв
				    	{
				    		case 0x05://начать новую игру немедленно
				    		{
				    			USART1_PutChar(team_color_to_symbol[armadaSystem.player.team_color]);
				    			bt_bandana_already_been_on=true;
				    			armadaSystem.player.health = armadaSystem.player.health_after_start;
/*
#ifndef 	COLOR_LCD
	  lcd_health_update();
#else
	  color_lcd_health_update();
#endif
*/
				    			armadaSystem.gun.clips = armadaSystem.gun.clips_after_start;


#ifdef RTC_Enable
				    			BKP_WriteBackupRegister(BKP_DR2, armadaSystem.gun.clips);
#endif

/*
#ifndef 	COLOR_LCD
									lcd_clips_update();
#else
									color_lcd_clips_update();
#endif
*/
				    			armadaSystem.gun.rounds = 0;
#ifdef RTC_Enable
				    			BKP_WriteBackupRegister(BKP_DR1, armadaSystem.gun.rounds);
#endif

/*
#ifndef 	COLOR_LCD
	lcd_rounds_update();
	if (!screen_auto_refresh) lcd8544_dma_refresh();
#else
	color_lcd_rounds_update();
#endif
*/


	//initGUI();
#ifdef DISPLAY_ENABLE
	display_init_gui();
#endif
	xSemaphoreGive(xGameOverSemaphore);//отдаем семафор
	  	  	  	armadaSystem.wav_player.type_of_sound_to_play = START_GAME;
	xSemaphoreGive(xWavPlayerManagerSemaphore);
								game_status = true;
#ifdef SI4432_ENABLE
	//RF_send_status_package();
	send_status_message_now=true;
#endif
								break;
							}
							case 0x00://"выключить" игрока
							{
								USART1_PutChar(team_color_to_symbol[armadaSystem.player.team_color]);
								if(game_status)//если в игре
								{
									game_over();
								}
								else /*if (armadaSystem.wav_player.type_of_sound_to_play!=GAME_OVER)*/
								{
									armadaSystem.wav_player.type_of_sound_to_play = GAME_OVER;//надо воспроизвести звук "игра закончена"
									xSemaphoreGive(xWavPlayerManagerSemaphore);
								}
#ifdef SI4432_ENABLE
	//RF_send_status_package();
	send_status_message_now=true;
#endif
								break;
							}
							default: break;
				    	}
					break;
				    }
				}

				USART1_FlushRxBuf();
				bt_header_received=false;
			}//[case RX_MESSAGE_COMPLETE://принято сообщение]
			break;
			case NOT_EVENT:		//нет ничего интересного ;-)
			{
			}
			break;
		}



}



trx_packet get_bt_packet_value(){ //Считываем данные из полученного пакета
trx_packet result;
uint8_t byte_tmp;

result.player_id = bt_rx_buffer[0];
byte_tmp = bt_rx_buffer[1];
byte_tmp = byte_tmp << 2; //избавляемся от бит цвета команды
byte_tmp = byte_tmp >> 4;
result.damage = damage_value[byte_tmp];
result.team_id = bt_rx_buffer[1]>>6;
return result;
}



tir_message get_bt_message_value(){ //Считываем сообщение из полученного пакета
tir_message result;
result.ID =  bt_rx_buffer[0];
result.param = bt_rx_buffer[1];
result.control_byte = bt_rx_buffer[2];
#if DEVICE_ROLE == TAG
switch(result.ID)
{
	case SYSTEM_DATA: //0x87 системные данные
	{

		switch(result.param)
		{
			case TAG_INIT_DATA: //0x02 данные для настройки тага
			{
				result.clone_data_union.tag_init_data.player_id=bt_rx_buffer[3];
				result.clone_data_union.tag_init_data.team_id=bt_rx_buffer[4];
				result.clone_data_union.tag_init_data.game_status = bt_rx_buffer[5];
				result.clone_data_union.tag_init_data.health = bt_rx_buffer[6];
				result.clone_data_union.tag_init_data.command = bt_rx_buffer[7];
				uint8_t* p_tmp;
				p_tmp = (uint8_t*)&result.clone_data_union.tag_init_data.game_session_ID;
				*p_tmp = bt_rx_buffer[8];
				p_tmp++;
				*p_tmp = bt_rx_buffer[9];
				p_tmp++;
				*p_tmp = bt_rx_buffer[10];
				p_tmp++;
				*p_tmp = bt_rx_buffer[11];
				result.clone_data_union.tag_init_data.Checksum = bt_rx_buffer[12];
			}
			break;


		}




	}
	break;
	default: break;

}
#endif
return result;
}

