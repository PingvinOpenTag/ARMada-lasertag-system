#ifndef RF_MODUL_H
#define RF_MODUL_H
#include "types.h"
#include "global_variables.h"

#ifdef SI4432_ENABLE


/*
 * 000 +1 dBm
 * 001 +2 dBm
 * 010 +5 dBm
 * 011 +8 dBm
 * 100 +11 dBm
 * 101 +14 dBm
 * 110 +17 dBm
 * 111 +20 dBm
 */



// RF stack enumerations
typedef enum _RF_ENUM
{
                RF_OK  = 0x00,  //function  response  parameters
                RF_ERROR_TIMING  = 0x01,
                RF_ERROR_PARAMETER  = 0x02,
                RF_PACKET_RECEIVED  = 0x03,
                RF_RX_FIFO_ALMOST_FULL = 0x04,
                RF_NO_PACKET = 0x05,
                RF_CRC_ERROR  = 0x06,
} RF_ENUM;



enum rfSetings{
// Data    Rate;  Freq Deviation; Receiver Bandwidth
dataRate2400BPS_DEV36KHZ=0,
// DR = 2.4kbps;Fdev = +-36kHz;BBBW = 75.2kHz;
dataRate4800BPS_DEV45KHZ = 1,
// DR = 4.8kbps;Fdev = +-45kHz;BBBW = 95.3kHz;
dataRate9600BPS_DEV45KHZ = 2,
// DR = 9.6kbps;     Fdev = +-45kHz;    BBBW = 112.8kHz;
dataRate10000BPS_DEV12KHZ = 3,
// DR = 10kbps;      Fdev = +-12kHz;    BBBW = 41.7kHz;
dataRate20000BPS_DEV12KHZ = 4,
// DR = 20kbps;      Fdev = +-12kHz;    BBBW = 45.2kHz;
dataRate40000BPS_DEV20KHZ = 5,
// DR = 40kbps;      Fdev = +-20kHz;    BBBW = 83.2kHz;
dataRate50000BPS_DEV25KHZ = 6,
// DR = 50kbps;      Fdev = +-25kHz;    BBBW = 112.8kHz;
dataRate100000BPS_DEV50KHZ = 7,
// DR = 100kbps;    Fdev = +-50kHz;    BBBW = 208kHz;
dataRate128000BPS_DEV64KHZ = 8,
// DR = 128kbps;    Fdev = +-64kHz;    BBBW = 269.3kHz;
};
typedef enum rfSetings tRfSettings;


enum rfTxPower{
	power_1dBm =0,
	power_2dBm,
	power_5dBm,
	power_8dBm,
	power_11dBm,
	power_14dBm,
	power_17dBm,
	power_20dBm

};

typedef enum rfTxPower tRF_Power;








void RF_modul_set_tx_power(tRF_Power);

void si4432_IQR_callback(void);

void RF_modul_Init(void);
bool  RF_modul_Rx(unsigned char *pBuf, unsigned char* pBytes, portTickType timeout);
bool RF_modul_TxBuf(unsigned char *pBuf, unsigned char bytes);


RF_ENUM RF_modul_SetRfParameters(tRfSettings setting);
void RF_modul_Init_with_parameters(tRfSettings setting, tRF_Power tx_power);


unsigned char RF_modul_get_RSSI(void);

void RF_modul_send_next_package(void);


void RF_modul_read_packages(void);

void rf_listen(void);

#endif //[#ifdef SI4432_ENABLE]


#endif //[#ifndef RF_MODUL_H]
