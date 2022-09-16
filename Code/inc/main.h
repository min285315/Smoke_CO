#ifndef _MAIN_H
#define _MAIN_H

#include "polling.h"
#include "qep_port.h"
#include "qassert.h"
#include "uart.h"
#include "sys_cfg.h"


//#include "stdio.h"











//#define SUPPORT_2_3V_VDD
#define SUPPORT_2_5V_VDD



/***********************************看门�?***************************************/
#define RELOAD_VALUE 254


/***********************************日期****************************************/
//product life time
#define LIFE_TIME                           (3650+100)//10*365   10 years+100 day(safe victor)
#define LIFE_TIME_MUTE                      (30)//  30 day
#define ENDOFLIFE_MUTE_TIME                 (7900)//    //22h 


/***********************************LCD****************************************/
#define LCD_SHOW_TOTAL_TIME  30 //10*30 =5min



/***********************************电池****************************************/
//pwr 
#define LOW_BATTERY_MUTE_TIME                   (3600)//    //10h 



#define SleepTimes_60S	((uint8_t)0x06)
#define SleepTimes_40S	((uint8_t)0x04)
#define SleepTimes_9Min	((uint8_t)0x6C)			//540s = 108*5s

#define TimeOut10ms		((uint8_t)1)
#define TimeOut50ms		((uint8_t)5)
#define TimeOut100ms	((uint8_t)10)
#define TimeOut500ms	((uint8_t)50)			//quanwu.xu  add 20190124
#define TimeOut1000ms   ((uint8_t)1000/*100*/)
#define TimeOut1500ms   ((uint8_t)150)
#define TimeOut4900ms   ((uint16_t)490)


#define SYSTEMTICKS     ((uint16_t)0xffff)
#define SYS_TIME_MAX	((uint8_t)0x08)

#define RTC_SLEEP_TIME_10s		((uint16_t)23030)
#define RTC_SLEEP_TIME_5s		((uint16_t)11515)
#define RTC_SLEEP_TIME_2s		((uint16_t)4606)
#define RTC_SLEEP_TIME_3s		((uint16_t)6909)
#define RTC_SLEEP_TIME_1500ms	((uint16_t)3454)
#define RTC_SLEEP_TIME_1s		((uint16_t)2303)
#define RTC_SLEEP_TIME_500ms	((uint16_t)1151)
#define RTC_SLEEP_TIME_200ms	((uint16_t)260)

#define LED_ON_DELAY_TIME_50	((uint8_t)50)
#define LED_ON_DELAY_TIME_400	((uint16_t)400)
#define LED_ON_DELAY_TIME_2000	((uint16_t)2000)

#define SOFTWARE_RESET()		{WWDG->CR &= ~(uint8_t)WWDG_CR_T6;}





//#define BSI_CERTIFY_FAIL_PROCESS		//quanwu.xu 20181218

/*************************************Test Fun Define************************************************/
//#define SUPPORT_CO_TEMP_TEST     		//co temperature test



#define SLEEP_TIME                              (10)                // 10s
#define SYSTEM_ONE_DAY_COUNT          (86400/SLEEP_TIME)      // 24h=24*60*60=86400/SLEEP_TIME=8640

#define CLOSE_LCD_BACKLIGHTTIME  5000

//�͹��Ľ���Flag
typedef enum _eSysActiveFlag
{
	eSysActiveFlag_NULL 		= (uint32_t)0,
	eSysActiveFlag_KeyPress		= (uint32_t)0x01<<0,
	eSysActiveFlag_KeyWait 		= (uint32_t)0x01<<1,
	eSysActiveFlag_WakeUp 		= (uint32_t)0x01<<2,
	eSysActiveFlag_Test 		= (uint32_t)0x01<<3,
	eActiveFlagNetCMDTimeOut 	= (uint32_t)0x01<<4,
	eActiveFlagSmokeMute 		= (uint32_t)0x01<<5,
	eActiveFlagSmokeMuteAlarmLed= (uint32_t)0x01<<6,
	eActiveFlagSmoke			= (uint32_t)0x01<<7,
	eActiveFlagCO 				= (uint32_t)0x01<<8,
	eActiveFlagCOBeep 			= (uint32_t)0x01<<9,
	eActiveFlagLed 				= (uint32_t)0x01<<10,
	eActiveFlagBeep				= (uint32_t)0x01<<11,
	eActiveFlagCOAlarmTimeout	= (uint32_t)0x01<<12,
	eActiveFlagCOBeepDelay		= (uint32_t)0x01<<13,
	eActiveFlagCloseLcdBackLight = (uint32_t)0x01<<14,
	eActiveFlagKeyTest			= (uint32_t)0x01<<15,
	eActiveFlagCoMuteAlarmLed	= (uint32_t)0x01<<16,
	eActiveFlagCancelAlarmLed	= (uint32_t)0x01<<17,
	eActiveFlagDevErr			= (uint32_t)0x01<<18,
	eActiveFlagCOAlarmTimeoutBeep = (uint32_t)0x01<<19,
	eActiveFlagLcd				= (uint32_t)0x01<<20,
	eActiveFlagSmokeBeepDelay	= (uint32_t)0x01<<21,
	eActiveFlagSmokeBeep		= (uint32_t)0x01<<22,
	eActiveFlagNet				= (uint32_t)0x01<<23,//NET
	eActiveFlagNetKeyTestGREENLED	= (uint32_t)0x01<<24,
	eActiveFlagNetTimeOut		= (uint32_t)0x01<<25,
	
	#ifdef SUPPORT_WIFI_NET_FUNCTION
	eActiveFlagWifiUpgradeTimeout		= (uint32_t)0x01<<26,
	eActiveFlagNetConnetTimeout		= (uint32_t)0x01<<27,
	eActiveFlagWifiTimeout		= (uint32_t)0x01<<28,
	#endif
	eActiveFlagPeakDisplayDelay		= (uint32_t)0x01<<29,
	eSysActiveFlag_All = (uint32_t)0x7FFFFFFF,
}eSysActiveFlag;

typedef enum
{
	//SystemEvent
	Q_eSystemEventIdle = Q_USER_SIG,
	Q_eSystemEventWakeup,
	eSystemEventSkipStart,
	eSystemEventEndLife,
	//pwr
	Q_eSystemEventLowBat,
	Q_eSystemEventLedBreath,
	eSystemEventLowPwrCancel,

	//Pair
	Q_eSystemEventPairFactory,
	Q_eSystemEventPairTimeout,
	Q_eSystemEventPairSuccess,
	Q_eSystemEventPairFail,
	
	//KeyEvent
	Q_eKeyEventKey0Clik,
	Q_eKeyEventKey0LongClik,
	Q_eKeyEventKey0FourClik,
	Q_eKeyEventKey1Clik,
	Q_eKeyEventKey1LongClik,
	Q_eKeyEventKey1LongClikRelease,
	//eSystemEventKey,	
	eSystemEventKeyTtestOver,
	eSystemEventKeyTestTimeout,
	
	//SmokeEvent
	eSystemEvenCalibrationSmoke,
	eSystemEvenHadCailSmoke,
	//eSystemEvenCalibrationMCUBatteryErr,
	Q_eSmokeEventCailSuccess,
	Q_eSmokeEventCailFail,
	Q_eSmokeEventAlarm,
	Q_eSmokeEventCancelAlarm,
	eSystemEventSmokeAlarmCancelTimeout,
	eSystemEventSmokeMuteTimeout,
	//Q_eSmokeEventMute,
	//Q_eSmokeEventMuteFinished,
	eSystemEventSmokeMuteExitTimeout,
	Q_eSmokeEventTest,
	Q_eSmokeEventTestCancel,
	Q_eSmokeEventTestFinished,
	//Q_eSmokeEventDevErr,
	Q_eSmokeEventDevErrRecovery,
	eSystemEventSmokeBeepOK,
	eSystemEventSmokeErr,
	eSystemEventKeyTtestSmokeTestOver,

	//COEvent
	eSystemEvenCalibrationCO,
	eSystemEvenCalibrationCOErr,
	eSystemEvenCalibrationCOOK,
	eSystemEvenCalibrationTempErr,
	eSystemEvenCalibrationCoBatteryErr,
	eSystemEventCOAlarmCancel,
	eSystemEventCOAlarm,
	eSystemEventCOErr,
	eSystemEventCOBeepOK,
	eSystemEventCOAlarmTimeout,
	eSystemEventCOMuteTimeout,
	eSystemEventCOMuteLedOK,
	eSystemEventKeyTtestCoTestOver,
	eSystemEventCOAlarmCancelTimeout,
	eSystemEventCoMuteExitTimeout,

	//lcd
	eSystemEventStartAnimationEnd,
	eSystemEventStartIndexEnd,
	#ifdef SUPPORT_WIFI_NET_FUNCTION
	//NET
	eWIFINETEventMute,
	eSystemEventAPWiFi,
	eSystemEventEZWiFi,
	eSystemEventExitPir,
	eSystemEventWifiPairStart,
	eSystemEventWifiPairSuccess,
	eSystemEventWifiPairFail,
	#endif
	//eNETEventCancleAlarm,
	eSystemEventPeakDisplay,


	Q_eEventMax,
}eSystemEvent;


//system
typedef enum
{ 
	eSystemMode_Idle = 0x00,
	eSystemMode_PairMaster,
	eSystemMode_PairSlaver,
	eSystemMode_PairFactory,
	eSystemMode_DeviceErr,
	eSystemMode_DeviceLowBat,
}eSystemMode;

//device state
typedef enum{
	eDeviceOK = 0,
	eDeviceErr ,
	eDevicePreEndofLife ,
	eDevicePreEndofLifeMute ,
	eDeviceEndofLife ,
	eDeviceStarting ,
	eDeviceLowBattery ,
	eDeviceLowBatteryMute ,
	eDeviceMax ,

	eDeviceCloseLcdBackLight ,

}eDeviceState;


#define MainTaskQueue_Length            (10)

typedef struct SystemQueueObject{
	QEvent  que;  		//Que
	uint32_t atrr;      //0:no use  1:used	
}SystemQueueTag;

typedef struct __SystemApiTag
{
	QFsm  super_;
	QEQueue eQueue__;
	QEvent  *pEventPoll;//event polling address

	#ifdef SUPPORT_LCD_FUNCTION
	uint8_t LcdSwitchFlag;
	#endif
	#ifdef SUPPORT_NET_FUNCTION
	uint8_t NetModeFlag;//NET Flag
	#endif
	uint16_t SysRtcSleepTime;
	uint16_t SystemTick;
	uint16_t DeviceState;
	uint16_t LifeTimeDays;//system Life time day
	uint16_t LowPwrSilenceFlag;//Low silence Flag
	uint16_t LowPwrSilence;// Low silence Time
	uint16_t SilenceTime;// Life end of time silence
	uint16_t LowBatCnt;
	uint32_t SlpTimeCnt;
	uint32_t SysActiveFlag;
	
	eSystemMode SysMode;
	FlagStatus PowerConsumptionCheckFlag;// factory  test  flag
	FlagStatus RTCWakeUpFlag;// RTC 定时睡眠时间
	//TmrHeader SysTimeHeader;
	//TmrInfo SysTimeArr[SYS_TIME_MAX];
}SystemApiTag;
extern SystemApiTag *pSystemApiTagObj; 
void SystemPostFifo(QEvent const *pEvt);

void SysTickAdd(void);
uint32_t SystemGetTick(void);
uint32_t SystemGetIdleNUm(void);
void SysTimeUpdate(void);
uint16_t SystemGetLifeDays(void);




void Delay(__IO uint16_t TimeCnt);
//void* Memcpy(void* dest, void* src, uint8_t Length);
void *Memset(void *dest, uint8_t set, uint8_t len);
void EnterActiveHaltMode();
void SetSysSleepFlag(eSysActiveFlag Flag);
void ResetSysSleepFlag(eSysActiveFlag Flag);
void SysIdleModeHandleEvent(SystemApiTag  *me,QEvent *e);
uint32_t SystemGetActiveFlag(uint32_t Flag);
void SystemKeyTestHandle(SystemApiTag  *me,QEvent *e);

void SystemStartHandle(SystemApiTag  *me,QEvent *e);


#ifdef SUPPORT_NET_FUNCTION
uint8_t IsNetEnable(void);
void SetNetConfig(uint8_t Flag);
uint8_t SystemGetNetMode(void);


#endif





#endif

