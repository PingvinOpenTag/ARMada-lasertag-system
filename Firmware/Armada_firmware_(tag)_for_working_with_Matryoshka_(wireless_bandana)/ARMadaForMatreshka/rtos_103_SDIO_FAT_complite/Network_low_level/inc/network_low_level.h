#ifndef NETWORK_LOW_LEVEL_H
#define NETWORK_LOW_LEVEL_H

#include "types.h"
#ifdef SI4432_ENABLE
#include <stdint.h>
#include "network_base_types.h"
#define RF_TX_BUFFER_SIZE 32
#define RF_RX_BUFFER_SIZE 16
#define RF_LAST_RECEIVED_PACKAGES_BUFFER_SIZE 64

uint16_t rf_generatePackageId(void);
unsigned char rf_GetTxCount(void);

unsigned char rf_GetLastRxCount(void);

//unsigned char rf_GetRxCount(void);
void rf_FlushTxBuf(void);
void rf_FlushLastRxBuf(void);

//void rf_FlushRxBuf(void);
bool rf_PutWaitingPackage(tWaitingPackage new_package); //положить пакет в буфер

bool rf_PutLastRxPackage(tLastReceivedPackagesItem new_package); //положить пакет в буфер

bool rf_GetWaitingPackage(tWaitingPackage* package_tmp);

bool rf_GetLastRxPackage(tLastReceivedPackagesItem* package_tmp);

bool rf_isLastPackageInBuf(tPackage lastPackage);

void rf_InitDefaultPackageTimings(tPackageTimings* packageTimings_tmp);
void rf_InitDefaultWaitingPackage(tWaitingPackage* waiting_package_tmp);
void rf_txDoneCallback(uint16_t tx_done_packageId);
//void rf_SortTxBuffByNextTransmission (void);
bool rf_send(tDeviceAddress target, tDeviceAddress sender,  uint8_t* data, uint16_t size,  bool waitForAck,  tPackageTimings timings);
bool rf_put_package_to_buffer(tDeviceAddress target, tDeviceAddress sender,  uint8_t* data, uint16_t size,  bool waitForAck,  tPackageTimings timings);
void rf_set_delivery_complite_flag(tPackageId package_id, tDeviceAddress target_address);

#endif //[#ifdef SI4432_ENABLE]
#endif //[#ifndef NETWORK_LOW_LEVEL_H]
