#ifndef __MILES_PROTOCOL_H
#define __MILES_PROTOCOL_H

#define IR_START_BIT_DURATION 2400	// Длительность Старт-Бита (в микросекундах)
#define IR_ONE_BIT_DURATION 1200	// Длительность Бита, соотретствующего единичке (в микросекундах)
#define IR_ZERO_BIT_DURATION 600	// Длительность Бита, соотретствующего нулю (в микросекундах)
#define IR_SPACE_DURATION 600		// Длительность Бита, соотретствующего интервалу между битами (в микросекундах)

#define IR_TOL                      20 //Допустимая погрешность при приеме ИК пакета (в процентах

// этот дефайн просто сравнивает значения с заданной точностью в процентах
#define checkVal(var,val,tol) (var>(val*(100-tol)/100) && var<(val*(100+tol)/100))

#define SAFE_TIME 50


//---------------------------------------------------------------------//

enum Team_Color {	//Определим перечисляемый тип для работы с цветом команды
	Red, 	//00 = Red
	Blue, 	//01 = Blue
	Yellow, //10 = Yellow
	Green}; //11 = Green

typedef enum Team_Color tteam_color;

//---------------------------------------------------------------------//
//Определим перечисляемый тип
//для работы с "уроном"
enum GunDamage {

	Damage_1,  //0000 = 1
	Damage_2,  //0001 = 2
	Damage_4,  //0010 = 4
	Damage_5,  //0011 = 5
	Damage_7,  //0100 = 7
	Damage_10, //0101 = 10
	Damage_15, //0110 = 15
	Damage_17, //0111 = 17
	Damage_20, //1000 = 20
	Damage_25, //1001 = 25
	Damage_30, //1010 = 30
	Damage_35, //1011 = 35
	Damage_40, //1100 = 40
	Damage_50, //1101 = 50
	Damage_75, //1110 = 75
	Damage_100 //1111 = 100
	};


typedef enum GunDamage tgun_damage;


#define Add_Health 0x80 //сообщение добавить жизни
#define Add_Rounds 0x81 //сообщение добавить патронов
#define Change_color 0xA9//сообщение сменить цвет команды
#define Command 0x83 //сообщение-команда
#define Valid_value 0xE8 //корректное значение контрольного байта сообщения
#define SYSTEM_DATA 0x87 //системные данные
#define CLONING_DATA 0x01 //данные для клонирования
#define TAG_INIT_DATA 0x02//данные для настройки тага

#endif /* __MILES_PROTOCOL_H */
