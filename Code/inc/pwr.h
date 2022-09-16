#ifndef _PWRH_
#define _PWRH_

#include "main.h"

struct BatteryObj{
	uint8_t State;//
	uint16_t LowBatCheckCnt;
	uint16_t BatterySample;
};
typedef struct BatteryObj BatteryAOTag;

#define BatteryState_FULL				(2850)		// >2.85v 		
#define BatteryState_TWO_THIRED			(2800)		// >2.80v			  
#define BatteryState_ONE_THIRED			(2700)		// > 2.7V	

#define LOW_BATTERY_HUSH_FORBID_THRESHOLD       (2650)    //(2.65v-50mv)/2=1.3v
#define BATTERY_CHECK_COUNT                                 (6)// 54s/SLEEP_TIME=6

typedef enum{
	eBatteryLevelNull = 0x00,
	eBatteryLevelOne,
	eBatteryLevelTwo,
	eBatteryLevelThree,	
	eBatteryLevelMAX,
}eBatteryState;

uint16_t GetBatteryData(void);
void BatteryInit(void);
uint8_t GetBatteryLowState(void);
void SetBatteryLowState(uint8_t State);
FlagStatus IsLowPwrErr(void);
void BatterCheck(void);
uint8_t CheckBatteryLevel(void);
uint8_t GetBatteryState(void);
#ifdef SUPPORT_LCD_FUNCTION
void BatteryLcdDisplay(void);
#endif


#endif