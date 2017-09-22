#ifndef NETWORK_TASKS_H
#define NETWORK_TASKS_H

#include "types.h"

#ifdef SI4432_ENABLE

#include "FreeRTOS.h"
#include "task.h"

void rf_tx_interrogate_task(void *pvParameters);
void rf_rx_interrogate_task(void *pvParameters);

#endif //[#ifdef SI4432_ENABLE]
#endif//[#ifndef NETWORK_TASKS_H]
