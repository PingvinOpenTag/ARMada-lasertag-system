/*
 * types.h
 *
 *  Created on: 28 июня 2016 г.
 *      Author: chubara_av
 */

#ifndef TYPES_H_
#define TYPES_H_


#ifndef bool

                        #define bool unsigned char

                        #define true 1

                        #define false 0

#endif

typedef void (*Callable)();

enum hex_record_type{
	/*'00'*/ Data_Record, //(запись, содержащая данные)
	/*'01'*/ End_of_File_Record, //(запись, сигнализирующая о конце файла)
	/*'02'*/ Extended_Segment_Address_Record, //(запись адреса расширенного сегмента)
	/*'03'*/ Start_Segment_Address_Record, //(запись адреса начала сегмента)
	/*'04'*/ Extended_Linear_Address_Record, //(запись расширенного линейного адреса)
	/*'05'*/ Start_Linear_Address_Record //(запись адреса начала линейного адреса)
};
typedef enum hex_record_type thex_record_type;


enum mem_page_status{
	not_erased,
	erased
};
typedef enum mem_page_status tmemoryPageStatus;

typedef struct hexRecordData{
	thex_record_type type;//тип записи
	uint32_t LoadOffset; //смещение
	uint8_t bytes;//количество байт
	uint8_t data[128];//буфер для данных
} thexRecordData;


#define RED ((1<<6))//красный
#define BLUE ((1<<4))//синий
#define GREEN ((1<<5))//зелёный
#define CYAN (BLUE|GREEN)//голубой
#define YELLOW (RED|GREEN)//желтый
#define MAGENTA (RED|BLUE)//пурпурный
#define WHITE (RED|GREEN|BLUE)//белый
#define BLACK (0)
//((1<<4)|(1<<5)|(1<<6))//черный
#define VIBRO_HP (1<<1)//пин вибромоторчика (высокая мощность)
#define VIBRO_LP (1<<1)//пин вибромоторчика (малая мощность)
#define RED_WO_VIBRO RED



#endif /* TYPES_H_ */
