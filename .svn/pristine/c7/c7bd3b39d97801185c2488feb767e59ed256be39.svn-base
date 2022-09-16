/****************************************************************************/
/*	File   :             Flash.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/29,16:59:15                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
//#include "Framework.h"
#include "Flash.h"
#include "base.h"

struct FlashApi{

	uint8_t SmokeCariFlag;
	uint8_t SmokeCariDataMsb;
	uint8_t SmokeCariDataLsb;
	//uint8_t SmokeTempValue;//温度值
	
	uint8_t COCariFlag;// Flag
	uint8_t COCariGradientMsb;// 5斜率
	uint8_t COCariGradientLsb;
	uint8_t COCariOffsetMsb; 
	uint8_t COCariOffsetLsb; 
	
	uint8_t LifeTimeMsb;//product life time restore
	uint8_t LifeTimeLsb;//9
	
	#ifdef SUPPORT_WIFI_NET_FUNCTION
	uint8_t eUpgradeFlag;
	uint8_t WifiFactory;
	#endif
	uint8_t HistCOPeakMsb;
	uint8_t HistCOPeakLsb;
	
	uint8_t NetModeFlag;    //NET Flag
	uint8_t HistPeakCount;
	uint8_t HistLowBatteryCount;
	uint8_t HistPowerResetCount;
	uint8_t HistCOAlarmEventCount;
	uint8_t HistSensorOpenFaultCount;//15
	uint8_t HistSensorShortFaultCount;
	uint8_t HistSmokeAlarmCount;
	uint8_t HistSmokeChamberFaultCount;

	uint8_t SmokeResetCounter;
	
	#ifdef SUPPORT_FILTER_FUNTION
	uint16_t SmokeFilterData[25];
	#endif
};


static FlashApiTag FlashObject;
static FlashApiTag *pFlashObject = NULL;




#if 0
static uint8_t OptionData[11] = {0xaa,0x00,0xff,0x00,0xff,0x02,0xfd,0x02,0x00,0x00,0x00};
#endif
void FlashLoad(void);
#if 0
void FlashLoadOption(void);
#endif


FlashApiTag * FlashInit(void)
{
	FlashLoad();
	pFlashObject = &FlashObject;
	return  pFlashObject;
}

/***************************************************/
/******************************************************************************
**Description:					FlashReadbyte													
** Parameters:                        												
**     Return:   																	
**     Author:                 song.jiang                                          
*******************************************************************************/

 uint8_t FlashReadbyte(uint32_t Address)
{
	return FLASH_ReadByte(Address);
}
/******************************************************************************
**Description:				FlashWriteByte														
** Parameters:                        												
**     Return:   																	
**     Author:                 song.jiang                                          
*******************************************************************************/
 
 void FlashWriteByte(uint32_t Address, uint8_t Data)
{
	FLASH_Unlock(FLASH_MemType_Data); 
	FLASH_ProgramByte(Address, Data);
	FLASH_Lock(FLASH_MemType_Data); 
}

/******************************************************************************
**Description:						FlashLoad												
** Parameters:                        												
**     Return:   																	
**     Author:                 song.jiang                                          
*******************************************************************************/

void FlashLoad(void)
{

	#ifdef SUPPORT_FILTER_FUNTION
	uint8_t  size_flashdata=0;
	uint8_t  flashdata=0;
	uint8_t i=0;
	uint8_t *pFlash = (uint8_t *)&FlashObject;
	
	size_flashdata =sizeof(FlashApiTag);
	printf("\r\nsize_flashdata=%d\r\n",size_flashdata);
	for (i = 0; i < (sizeof(FlashApiTag)); i++)
	{
		if(i%4==0)
		{
			printf("\r\n");
		}
		flashdata = FlashReadbyte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+i);
		pFlash[i] =flashdata;
		printf("0x%x     ",flashdata);
	}
	
	#else
	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t *pFlash = (uint8_t *)&FlashObject;
	printf("\nFlash :%d\n",sizeof(FlashApiTag));
	
	for (i = 0; i < (sizeof(FlashApiTag)>>4); ++i)
	{
//		IWDG_ReloadCounter();
	 	printf("0x%x:",i*16);
		for(j=0; j<16; j++)
        {
//	        IWDG_ReloadCounter();
			pFlash[i*16+j] = FlashReadbyte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+i*16+j);
			printf("0x%x ",pFlash[i*16+j]);
		}
		printf("\n");
	}
	#endif
	printf("\n");
}


#if 0
void FlashLoadOption(void)
{
    uint8_t i = 0;
	for (i = 0; i < sizeof(OptionData); ++i)
	{
		FLASH_Unlock(FLASH_MemType_Data);
		FLASH_ProgramOptionByte(FLASH_OPTION_BYTES_START_PHYSICAL_ADDRESS+i, OptionData[i]);
		FLASH_Lock(FLASH_MemType_Data);
	}
}
#endif
#ifdef  SUPPORT_WIFI_NET_FUNCTION
void FlashSeteUpgradeFlag(uint8_t State)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->eUpgradeFlag = State; 
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,eUpgradeFlag), pFlash->eUpgradeFlag);
}

uint8_t FlashGeteUpgradeFlag(void)
{
	FlashApiTag *pFlash = &FlashObject;
	return pFlash->eUpgradeFlag;
}

void FlashSetWifiFactory(uint8_t State)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->WifiFactory = State; 
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,WifiFactory), pFlash->WifiFactory);
}

uint8_t FlashGetWifiFactory(void)
{
	FlashApiTag *pFlash = &FlashObject;
	return pFlash->WifiFactory;
}

#endif
uint8_t FlashGetNetState(void)
{
	FlashApiTag *pFlash = &FlashObject;
	return pFlash->NetModeFlag;
}

void FlashSetNetState(uint8_t State)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->NetModeFlag = State; 
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,NetModeFlag), State);
}


uint8_t FlashGetSmokeState(void)
{
	FlashApiTag *pFlash = &FlashObject;
	return pFlash->SmokeCariFlag;
}

void FlashSetSmokeState(uint8_t State)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->SmokeCariFlag = State; 
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,SmokeCariFlag), State);
}
void FlashSetSmokeData(uint16_t Data)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->SmokeCariDataMsb = Data>>8; 
	pFlash->SmokeCariDataLsb = Data&0xff;

	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,SmokeCariDataMsb), pFlash->SmokeCariDataMsb);
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,SmokeCariDataLsb), pFlash->SmokeCariDataLsb);
}

uint16_t FlashGetSmokeData(void)
{
	FlashApiTag *pFlash = &FlashObject;
	uint16_t Data = (uint16_t)(pFlash->SmokeCariDataMsb);
	Data = (Data<<8) + pFlash->SmokeCariDataLsb; 
    return Data;
}

uint8_t FlashGetCOState(void)
{
	FlashApiTag *pFlash = &FlashObject;
	return pFlash->COCariFlag;
}

void FlashSetCOState(uint8_t State)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->COCariFlag = State; 
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,COCariFlag), State);

}
uint16_t FlashGetCOGradient(void)
{
	FlashApiTag *pFlash = &FlashObject;
	uint16_t Data = (uint16_t)(pFlash->COCariGradientMsb);
	Data = (Data<<8) + pFlash->COCariGradientLsb; 

    return Data;
}

void FlashSetCOGradient(uint16_t Data)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->COCariGradientLsb = Data&0xff;
	pFlash->COCariGradientMsb = Data>>8; 
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,COCariGradientMsb), pFlash->COCariGradientMsb);
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,COCariGradientLsb), pFlash->COCariGradientLsb);
}
uint16_t FlashGetCOOffset(void)
{
	FlashApiTag *pFlash = &FlashObject;
	uint16_t Data = (uint16_t)(pFlash->COCariOffsetMsb);
	Data = (Data<<8) + pFlash->COCariOffsetLsb;
    return Data;
}

void FlashSetCOOffset(uint16_t Data)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->COCariOffsetLsb = Data&0xff;
	pFlash->COCariOffsetMsb = Data>>8; 
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,COCariOffsetMsb), pFlash->COCariOffsetMsb);
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,COCariOffsetLsb), pFlash->COCariOffsetLsb);
}

uint16_t FlashGetLifeTime(void)
{
	FlashApiTag *pFlash = &FlashObject;
	uint16_t Data = (uint16_t)(pFlash->LifeTimeMsb);
	Data = (Data<<8) + pFlash->LifeTimeLsb;
    return Data;
}

void FlashSetLifeTime(uint16_t Data)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->LifeTimeLsb = Data&0xff;
	pFlash->LifeTimeMsb = Data>>8; 
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,LifeTimeMsb), pFlash->LifeTimeMsb);
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,LifeTimeLsb), pFlash->LifeTimeLsb);
}
	

void FlashHistStoragePeak(void)
{
	FlashApiTag *pFlash = &FlashObject;

    if(pFlash->HistPeakCount < U8_MAX)
    {
        pFlash->HistPeakCount++;
    }
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistPeakCount), pFlash->HistPeakCount);

}
void FlashHistStorageLowBat(void)
{

	FlashApiTag *pFlash = &FlashObject;

    if(pFlash->HistLowBatteryCount < U8_MAX)
    {
        pFlash->HistLowBatteryCount++;
    }
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistLowBatteryCount), pFlash->HistLowBatteryCount);

}
void FlashHistStorageReset(void)
{
	FlashApiTag *pFlash = &FlashObject;

    if(pFlash->HistPowerResetCount < U8_MAX)
    {
        pFlash->HistPowerResetCount++;
    }
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistPowerResetCount), pFlash->HistPowerResetCount);

}
void FlashHistStorageCOAlarm(void)
{
	FlashApiTag *pFlash = &FlashObject;

    if(pFlash->HistCOAlarmEventCount < U8_MAX)
    {
        pFlash->HistCOAlarmEventCount++;
    }
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistCOAlarmEventCount), pFlash->HistCOAlarmEventCount);

}
void FlashHistStorageSmokeAlarm(void)
{
	FlashApiTag *pFlash = &FlashObject;

    if(pFlash->HistSmokeAlarmCount < U8_MAX)
    {
        pFlash->HistSmokeAlarmCount++;
    }
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistSmokeAlarmCount), pFlash->HistSmokeAlarmCount);

}

void FlashHistStorageSensorOpen(void)
{

	FlashApiTag *pFlash = &FlashObject;

    if(pFlash->HistSensorOpenFaultCount < U8_MAX)
    {
        pFlash->HistSensorOpenFaultCount++;
    }
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistSensorOpenFaultCount), pFlash->HistSensorOpenFaultCount);

}
void FlashHistStorageSensorShort(void)
{
	FlashApiTag *pFlash = &FlashObject;

    if(pFlash->HistSensorShortFaultCount < U8_MAX)
    {
        pFlash->HistSensorShortFaultCount++;
    }
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistSensorShortFaultCount), pFlash->HistSensorShortFaultCount);

}

void FlashSetCoData(uint16_t Data)
{
	FlashApiTag *pFlash = &FlashObject;
	pFlash->HistCOPeakLsb = Data&0xff;
	pFlash->HistCOPeakMsb = Data>>8; 
	
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistCOPeakMsb), pFlash->HistCOPeakMsb);
	FlashWriteByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+offsetof(FlashApiTag,HistCOPeakLsb), pFlash->HistCOPeakLsb);
}

uint16_t FlashGetCoData(void)
{
	FlashApiTag *pFlash = &FlashObject;
	uint16_t Data = (uint16_t)(pFlash->HistCOPeakMsb);
	Data = (Data<<8) + pFlash->HistCOPeakLsb; 
    return Data;
}



