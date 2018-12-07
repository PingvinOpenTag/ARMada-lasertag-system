#include  "network_low_level.h"
#include "network_base_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "global_variables.h"
#include <string.h>
#include <stdint.h>
#include "types.h"

#ifdef SI4432_ENABLE

//передающий буфер
volatile tWaitingPackage rf_TxBuf[RF_TX_BUFFER_SIZE];

//буфер недавно принятых пакетов

volatile tLastReceivedPackagesItem rf_lastRxBuff[RF_LAST_RECEIVED_PACKAGES_BUFFER_SIZE];

volatile unsigned char rf_txBufTail = 0;
volatile unsigned char rf_txBufHead = 0;
volatile unsigned char rf_txCount = 0;

volatile unsigned char rf_lastRxBufTail = 0;
volatile unsigned char rf_lastRxBufHead = 0;
volatile unsigned char rf_lastRxCount = 0;

/*
//приёмный буфер
volatile tPackage rf_RxBuf[RF_RX_BUFFER_SIZE];

volatile unsigned char rf_rxBufTail = 0;
volatile unsigned char rf_rxBufHead = 0;
volatile unsigned char rf_rxCount = 0;
*/


uint16_t rf_generatePackageId()
{
static volatile uint16_t last_ID = 3;
	if (last_ID==0xFFFF) last_ID = 3;
	return last_ID++;
}


unsigned char rf_GetTxCount(void)
{
  return  rf_txCount;
}


unsigned char rf_GetLastRxCount(void){
	return rf_lastRxCount;
}



/*
unsigned char rf_GetRxCount(void)
{
  return  rf_rxCount;
}
*/

//"очищает" передающий буфер
void rf_FlushTxBuf(void)
{
	rf_txBufTail = 0;
	rf_txBufHead = 0;
	rf_txCount = 0;
}


void rf_FlushLastRxBuf(void)
{
	rf_lastRxBufTail=0;
	rf_lastRxBufHead=0;
	rf_lastRxCount=0;
}


/*
//"очищает" приёмный буфер
void rf_FlushRxBuf(void)
{
	rf_rxBufTail = 0;
	rf_rxBufHead = 0;
	rf_rxCount = 0;
}
*/
bool rf_PutLastRxPackage(tLastReceivedPackagesItem new_package) //положить ghbyznsq пакет в буфер
{

	 if (rf_lastRxCount < RF_LAST_RECEIVED_PACKAGES_BUFFER_SIZE){    //если в буфере еще есть место
		 rf_lastRxBuff[rf_lastRxBufTail] = new_package; //помещаем в него символ
		 rf_lastRxCount++;                   //инкрементируем счетчик символов
		 rf_lastRxBufTail++;                 //и индекс хвоста буфера
		 if (rf_lastRxBufTail == RF_LAST_RECEIVED_PACKAGES_BUFFER_SIZE) rf_lastRxBufTail = 0;
		 return true;
		 }
	 return false;
}



bool rf_PutWaitingPackage(tWaitingPackage new_package) //положить пакет в буфер
{

		 if (rf_txCount < RF_TX_BUFFER_SIZE){    //если в буфере еще есть место
			 rf_TxBuf[rf_txBufTail] = new_package; //помещаем в него символ
			 rf_txCount++;                   //инкрементируем счетчик символов
			 rf_txBufTail++;                 //и индекс хвоста буфера
			 if (rf_txBufTail == RF_TX_BUFFER_SIZE) rf_txBufTail = 0;
			 return true;
			 }
		 return false;
}



//чтение буфера
bool rf_GetWaitingPackage(tWaitingPackage* package_tmp)
{
	//tWaitingPackage package_tmp;
  if (rf_txCount > 0){                     //если приемный буфер не пустой
	 *package_tmp = rf_TxBuf[rf_txBufHead];        //прочитать из него символ
    rf_txCount--;                          //уменьшить счетчик символов
    rf_txBufHead++;                        //инкрементировать индекс головы буфера
    if (rf_txBufHead == RF_TX_BUFFER_SIZE) rf_txBufHead = 0;
    return true;                         //вернуть прочитанный символ
  }
  return false;
}


bool rf_GetLastRxPackage(tLastReceivedPackagesItem* package_tmp){
	//tWaitingPackage package_tmp;
  if (rf_lastRxCount > 0){                     //если приемный буфер не пустой
	 *package_tmp = rf_lastRxBuff[rf_lastRxBufHead];        //прочитать из него символ
    rf_lastRxCount--;                          //уменьшить счетчик символов
    rf_lastRxBufHead++;                        //инкрементировать индекс головы буфера
    if (rf_lastRxBufHead == RF_LAST_RECEIVED_PACKAGES_BUFFER_SIZE) rf_lastRxBufHead = 0;
    return true;                         //вернуть прочитанный символ
  }
  return false;

}

void rf_InitDefaultPackageTimings(tPackageTimings* pPackageTimings){

	pPackageTimings->timeout = (defaultTimeout/1000000)*TIC_FQR;
	pPackageTimings->resendTime = (defaultResendTime*TIC_FQR)/1000000;
	pPackageTimings->resendTimeDelta = (defaultResendTimeDelta*TIC_FQR)/1000000;
	pPackageTimings->infiniteResend = false;

}


extern const tDeviceAddress my_rf_net_address;

void rf_InitDefaultWaitingPackage(tWaitingPackage* waiting_package_tmp){

	 uint32_t time_tmp;
	 time_tmp =  xTaskGetTickCount();
	 rf_InitDefaultPackageTimings(&waiting_package_tmp->timings);
	 waiting_package_tmp->package.details.packageId = rf_generatePackageId();
	 waiting_package_tmp->package.details.needAck = 1;

	 waiting_package_tmp->wasCreated =time_tmp;
	 waiting_package_tmp->nextTransmission = time_tmp;
	 waiting_package_tmp->isBroadcast = true;
	 waiting_package_tmp->callback =  '\0'/*rf_txDoneCallback*/;
	 waiting_package_tmp->package.sender=my_rf_net_address;
}


void rf_txDoneCallback(uint16_t tx_done_packageId){


}

/*
#define MAXSTACK 256//1024//2048		// максимальный размер стека


static volatile long lbstack[MAXSTACK], ubstack[MAXSTACK]; // стек запросов
                       // каждый запрос задается парой значений,
                       // а именно: левой(lbstack) и правой(ubstack)
                       // границами промежутка



void rf_SortTxBuffByNextTransmission (void){

	long size;
	size = rf_GetTxCount();

    long i, j;   			// указатели, участвующие в разделении

      long lb, ub;  		// границы сортируемого в цикле фрагмента

      long stackpos = 1;   	// текущая позиция стека
      long ppos;            // середина массива
      tWaitingPackage pivot;              // опорный элемент
      tWaitingPackage temp;
      lbstack[1] = 0;
      ubstack[1] = size-1;

      do {

         // Взять границы lb и ub текущего массива из стека.

         lb = lbstack[ stackpos ];
         ub = ubstack[ stackpos ];
         stackpos--;

         do {
           // Шаг 1. Разделение по элементу pivot

           ppos = ( lb + ub ) >> 1;
           i = lb; j = ub; pivot = rf_TxBuf[ppos];

           do {
             while ( rf_TxBuf[i].nextTransmission < pivot.nextTransmission ) i++;
             while ( pivot.nextTransmission < rf_TxBuf[j].nextTransmission ) j--;

             if ( i <= j ) {
               temp = rf_TxBuf[i]; rf_TxBuf[i] = rf_TxBuf[j]; rf_TxBuf[j] = temp;
               i++; j--;
             }
           } while ( i <= j );

           // Сейчас указатель i указывает на начало правого подмассива,
           // j - на конец левого (см. иллюстрацию выше), lb ? j ? i ? ub.
           // Возможен случай, когда указатель i или j выходит за границу массива

           // Шаги 2, 3. Отправляем большую часть в стек  и двигаем lb,ub

           if ( i < ppos ) {     // правая часть больше

             if ( i < ub ) {     //  если в ней больше 1 элемента - нужно
               stackpos++;       //  сортировать, запрос в стек
               lbstack[ stackpos ] = i;
               ubstack[ stackpos ] = ub;
             }
             ub = j;             //  следующая итерация разделения
                                 //  будет работать с левой частью

           } else {       	    // левая часть больше

             if ( j > lb ) {
               stackpos++;
               lbstack[ stackpos ] = lb;
               ubstack[ stackpos ] = j;
             }
             lb = i;
           }

         } while ( lb < ub );        // пока в меньшей части более 1 элемента

       } while ( stackpos != 0 );    // пока есть запросы в стеке
}
*/

bool rf_send(tDeviceAddress target, tDeviceAddress sender,  uint8_t* data, uint16_t size,  bool waitForAck,  tPackageTimings timings){
	bool result=false;
	tWaitingPackage rf_package_tmp;
	rf_package_tmp.package.target = target;
	rf_package_tmp.package.sender = sender;
	rf_package_tmp.package.details.needAck = waitForAck;
	rf_package_tmp.timings = timings;
	memcpy (rf_package_tmp.package.payload, data, size);
	 if (size<payloadLength)
	        {
	            memset(&(rf_package_tmp.package.payload[size]), 0, payloadLength-size);
	}
	 rf_package_tmp.package.details.packageId = rf_generatePackageId();
	 rf_package_tmp.isDeliveryComplite = false;
	 rf_package_tmp.wasCreated = xTaskGetTickCount();
	 rf_package_tmp.nextTransmission =  rf_package_tmp.wasCreated;
	 if (xSemaphoreTake(rf_tx_buff_Semaphore, TIC_FQR*2/*portMAX_DELAY*/ )== pdTRUE)//ждем 2 секунды прерывание Si4432
	{


		result=rf_PutWaitingPackage(rf_package_tmp);
//		if (rf_GetTxCount()>1) 	rf_SortTxBuffByNextTransmission(); //упорядочиваем по времени отправки
		xSemaphoreGive(rf_tx_buff_Semaphore);
		xSemaphoreGive( Si4432_IQR_Semaphore);
		return result;
	}
	else result =  false;//не дождались семафора

return result;

}

bool rf_put_package_to_buffer(tDeviceAddress target, tDeviceAddress sender,  uint8_t* data, uint16_t size,  bool waitForAck,  tPackageTimings timings){
	bool result=false;
		tWaitingPackage rf_package_tmp;
		rf_package_tmp.package.target = target;
		rf_package_tmp.package.sender = sender;
		rf_package_tmp.package.details.needAck = waitForAck;
		rf_package_tmp.timings = timings;
		memcpy (rf_package_tmp.package.payload, data, size);
		 if (size<payloadLength)
		        {
		            memset(&(rf_package_tmp.package.payload[size]), 0, payloadLength-size);
		}
		 rf_package_tmp.package.details.packageId = rf_generatePackageId();
		 rf_package_tmp.isDeliveryComplite = false;
		 rf_package_tmp.wasCreated = xTaskGetTickCount();
		 rf_package_tmp.nextTransmission =  rf_package_tmp.wasCreated;



			result=rf_PutWaitingPackage(rf_package_tmp);
//			if (rf_GetTxCount()>1) 	rf_SortTxBuffByNextTransmission(); //упорядочиваем по времени отправки
			return result;


	return result;
}

bool rf_isLastPackageInBuf(tPackage lastPackage)
{
	bool result = false;
	if(rf_GetLastRxCount())//если буфер не пустой
	{

		if(rf_lastRxBufTail > rf_lastRxBufHead){
			for (unsigned char i = rf_lastRxBufHead; i < rf_lastRxBufTail; i++)
			{
				if (rf_lastRxBuff[i].id ==lastPackage.details.packageId)//если ID пакета совпадает
				{
					//и если совпадают адреса
					if ((rf_lastRxBuff[i].sender.address[0]==lastPackage.sender.address[0])&&(rf_lastRxBuff[i].sender.address[1]==lastPackage.sender.address[1])&&(rf_lastRxBuff[i].sender.address[2]==lastPackage.sender.address[2]))
					{
						result=true;
					}
				}
			}



		}
		else
		{

			for (unsigned char i = rf_lastRxBufHead; i < RF_LAST_RECEIVED_PACKAGES_BUFFER_SIZE; i++)
			{
				if (rf_lastRxBuff[i].id==lastPackage.details.packageId)//если ID пакета совпадает
				{
					//и если совпадают адреса
					if ((rf_lastRxBuff[i].sender.address[0]==lastPackage.sender.address[0])&&(rf_lastRxBuff[i].sender.address[1]==lastPackage.sender.address[1])&&(rf_lastRxBuff[i].sender.address[2]==lastPackage.sender.address[2]))
					{
						result = true;
					}
				}
			}
			for (unsigned char i = 0; i < rf_lastRxBufTail; i++)
			{
				if (rf_lastRxBuff[i].id==lastPackage.details.packageId)//если ID пакета совпадает
				{
					//и если совпадают адреса
					if ((rf_lastRxBuff[i].sender.address[0]==lastPackage.sender.address[0])&&(rf_lastRxBuff[i].sender.address[1]==lastPackage.sender.address[1])&&(rf_lastRxBuff[i].sender.address[2]==lastPackage.sender.address[2]))
					{
						result = true;
					}
				}
			}
		}
	}

return result;

}



void rf_set_delivery_complite_flag(tPackageId package_id, tDeviceAddress target_address){

	if (xSemaphoreTake(rf_tx_buff_Semaphore, TIC_FQR*2/*portMAX_DELAY*/ )== pdTRUE)//ждем 2 секунды прерывание Si4432
	{


		if(rf_GetTxCount())
		{
			if(rf_txBufTail>rf_txBufHead)
			{
				for (unsigned char i = rf_txBufHead; i < rf_txBufTail; i++)
				{
					if (rf_TxBuf[i].package.details.packageId ==package_id)//если ID пакета совпадает
					{
						//и если совпадают адреса
						if ((rf_TxBuf[i].package.target.address[0]==target_address.address[0])&&(rf_TxBuf[i].package.target.address[1]==target_address.address[1])&&(rf_TxBuf[i].package.target.address[2]==target_address.address[2]))
						{
							rf_TxBuf[i].isDeliveryComplite=true;
						}
					}
				}
			}
			else
			{
				for (unsigned char i = rf_txBufHead; i < RF_TX_BUFFER_SIZE; i++)
				{
					if (rf_TxBuf[i].package.details.packageId ==package_id)//если ID пакета совпадает
					{
						//и если совпадают адреса
						if ((rf_TxBuf[i].package.target.address[0]==target_address.address[0])&&(rf_TxBuf[i].package.target.address[1]==target_address.address[1])&&(rf_TxBuf[i].package.target.address[2]==target_address.address[2]))
						{
							rf_TxBuf[i].isDeliveryComplite=true;
						}
					}
				}
				for (unsigned char i = 0; i < rf_txBufTail; i++)
				{
					if (rf_TxBuf[i].package.details.packageId ==package_id)//если ID пакета совпадает
					{
						//и если совпадают адреса
						if ((rf_TxBuf[i].package.target.address[0]==target_address.address[0])&&(rf_TxBuf[i].package.target.address[1]==target_address.address[1])&&(rf_TxBuf[i].package.target.address[2]==target_address.address[2]))
						{
							rf_TxBuf[i].isDeliveryComplite=true;
						}
					}
				}
			}
		}



		xSemaphoreGive(rf_tx_buff_Semaphore);
	}

}


#endif //[#ifdef SI4432_ENABLE]
