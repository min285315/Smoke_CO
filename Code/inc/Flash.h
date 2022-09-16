/****************************************************************************/
/*	File   :             Flash.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/29,16:59:48                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#ifndef __FLASH_H__
#define __FLASH_H__
#include "stm8l15x_flash.h"




typedef struct FlashApi FlashApiTag;
typedef enum
{
      
    HIST_STORAGE_PEAK_COUNT=0x00,
    HIST_STORAGE_LOW_BATTERY_COUNT,
    HIST_STORAGE_POWER_RESET_COUNT,
    HIST_STORAGE_CO_ALARM_EVENT_DATA,
    HIST_STORAGE_SENSOR_OPEN_FAULT_COUNT,
    HIST_STORAGE_SENSOR_SHORT_FAULT_COUNT,
    HIST_STORAGE_SMOKE_ALARM_COUNT,
    HIST_STORAGE_SMOKE_CHAMBER_FAULT_COUNT,

    HIST_STORAGE_MAX,                     
}Hist_Storage_TYPE;


FlashApiTag * FlashInit(void);
uint8_t FlashReadbyte(uint32_t Address);
void FlashWriteByte(uint32_t Address, uint8_t Data);

void FlashLoad(void);
uint8_t FlashGetFactoryState(void);
uint8_t FlashGetSmokeState(void);
void FlashSetSmokeState(uint8_t State);
uint8_t FlashGetCOState(void);
void FlashSetCOState(uint8_t State);
void FlashSetSmokeData(uint16_t Data);
uint16_t FlashGetSmokeData(void);
uint16_t FlashGetCOGradient(void);
void FlashSetCOGradient(uint16_t Data);
uint16_t FlashGetCOOffset(void);
void FlashSetCOOffset(uint16_t Data);
uint16_t FlashGetLifeTime(void);
void FlashSetLifeTime(uint16_t Data);
void FlashSetNetState(uint8_t State);
uint8_t FlashGetNetState(void);
void FlashSeteUpgradeFlag(uint8_t State);
uint8_t FlashGeteUpgradeFlag(void);
void FlashSetWifiFactory(uint8_t State);
uint8_t FlashGetWifiFactory(void);
void FlashSetCoData(uint16_t Data);
uint16_t FlashGetCoData(void);





#endif

