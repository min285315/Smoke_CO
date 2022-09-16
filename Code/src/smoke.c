#include "main.h"
#include "base.h"
#include "smoke.h"
#include "co.h"
#include "SceneInstance.h"
#include "uart.h"
#include "pwr.h"
#include "lcd.h"
#include "RF.h"
#include "led.h"
#include "beep.h"
#include "adc.h"
#include "protocol.h"
#include "WIFIUpdata.h"
#include "wifi.h"


//#define SUPPORT_MATCH_SAMPLE_TIMING


typedef struct _SmokeTag{
	//uint8_t TempValue;
	uint16_t SmokeDarkValue;
	uint16_t SmokeLightValue;
	uint16_t SmokeDiffValue;
	uint16_t SmokeSensitiveValue;
	eSmokeState SmokeState;
}SmokeTag;

static QEvent SmokeEvent;
static SmokeTag SmokeTagObj;

//for new tmp compensation  
const uint16_t TempRate[91]={
	1135,1130,1125,1123,1115, 	//-20 ~ -16
	1110,1108,1103,1100,1055,	//-15 ~ -11 
	1025,1015,1014,1010,1007,	//-10 ~ -6 	
	1002,1000,997, 995, 993,	//-5 ~ -1
	990, 
	989, 987, 986, 985, 984,  	//1 ~ 5
	982, 981, 980, 980, 980,	 	//6 ~ 10
	980, 980, 980, 981, 981,	 	//11 ~ 15
	982, 983, 984, 986, 988,	 	//16 ~ 20
	990, 992, 994, 996, 1000,	 	//21 ~ 25
	1002, 1006, 1010, 1014, 1016, 	//26 ~ 30
	1022, 1026, 1030, 1036, 1039,	//31 ~ 35
	1047, 1053, 1059, 1065, 1071,	//36 ~ 40
	1078,1085,1091,1098,1105,		//41 ~ 45
	1113,1121,1127,1135,1142,		//46 ~ 50
	1151,1159,1166,1174,1183,		//51 ~ 55
	1183,1190,1205,1222,1237,		//56 ~ 60
	1239,1247,1255,1264,1273,		//61 ~ 65
	1285,1299,1313,1329,1355,		//66 ~ 70
};


#define VERIFY_SMOKE_SAMPLE_TIMES		((uint8_t)0x10)
#define ADC_SAMPLE_DELAY_TIME_1			((uint8_t)1)
#define ADC_SAMPLE_DELAY_TIME_2			((uint8_t)2)

//#define SMOKE_ALARM_LEVEL 				((uint16_t)450)
#define EMC_TEST_LEVEL					((uint8_t)70)
#define DUST_COMPENSATE_MAX_VALUE		((uint16_t)0xFFF)
#define DUST_COMPENSATE_LEVEL			((uint8_t)32)
#define DUST_COMPENSATE_INTERVAL_TIMES	((uint8_t)1200)



#define VERIFY_TEMP_MIX				((uint8_t)(15+20))
#define VERIFY_TEMP_MAX				((uint8_t)(32+20))


#define BAT_CHECK_INTERVAL_TIME_12H	((uint16_t)4320)			
#define BAT_CHECK_INTERVAL_TIME_24H	((uint16_t)8640)			



//Test
#define SMOKE_TEST_LCD_TIME         	  (450)
#define SMOKE_CALIBRATION_LCD_TIME        (650)//MS
#define SMOKE_BEEP_TIME                   (1000)//MS

//dev
#define SMOKE_ERR_INDEX_TIME              (200)
#define SMOKE_MUTE_TIME_NUM               (108)//5S*108
#define SMOKE_MUTE_TIME                   (5000)//MS

#define SMOKE_ERR_TIME                    (55) //S
#define SMOKE_CHECK_TIME         		  (3000)


#define SMOKE_KEY_TEST_DELAY_TIME         (100)//MS

//校机
#define SMOKE_CALIBRATION_ERR_TIME        (10)
#define SMOKE_CALIBRATION_TIME            (2000)//MS
/*
uint8_t SensorGetTemperature(void)
{
    return SmokeTagObj.TempValue; 
}
*/
#ifdef SUPPORT_LCD_FUNCTION
//dzs
static void PollingSmokeCalibrationLcdDisplay(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint32_t i = 0;
	static uint32_t j = 0;
	
	switch(eCmd)
	{
		case eInsertTimer:
			LCD_Clear();//	
			break;
	
		case eTimeOut:
			if(j%3==0)
			{		
				LCD_Clear();
			}
			if(i%4==3)
			{

			}else
			{			
				LCD_SystemStartAnimation(j%3);//
				j++;
			}
			++i;
			break;
			
		case eDelTimer:
			j = 0;
			i = 0;			
			LCD_Clear();//	
			break;	
		default:
			break;
	}
}
#endif




/**********************************************************************/
//Description:		PollingTestTimeout()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void PollingTestTimeout(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t TimeoutCnt = 0;
	Handle = Handle;	

	switch(eCmd)
	{
		case eInsertTimer:
			TimeoutCnt = 0;
			break;
			
		case eTimeOut:
			if(TimeoutCnt++ > 5)
			{
				R_LED_TURN_OFF();
				BEEP_TURN_OFF();
				SmokeEvent.sig = Q_eSmokeEventTestFinished;
				SystemPostFifo(&SmokeEvent);
			}
			else
			{
				R_LED_TOGGLE();
				BEEP_TOGGLE();
			}
			break;

		case eDelTimer:
			R_LED_TURN_OFF();
			BEEP_TURN_OFF();
			break;

		default:
			break;
	}	
}


/**********************************************************************/
//Name:			LoadSmokerData()
//Description:																  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void LoadSmokerData(void)
{ 
	uint16_t SmokeAlarmLv = SMOKE_ALARM_LEVEL;
	//SmokeTagObj.TempValue = FLASH_ReadByte(FLASH_DATA_EEPROM_ADDR_TEMPTURE);
	SmokeTagObj.SmokeSensitiveValue = FLASH_ReadByte(FLASH_DATA_EEPROM_ADDR_SENSORTIV_DATA_MSB);
	SmokeTagObj.SmokeSensitiveValue <<= 8;
	SmokeTagObj.SmokeSensitiveValue |= FLASH_ReadByte(FLASH_DATA_EEPROM_ADDR_SENSORTIV_DATA_LSB);

	printf("Senitive = %d,AlarmLevel = %d\n", SmokeTagObj.SmokeSensitiveValue,SmokeAlarmLv);
	
}



/**********************************************************************/
//Name:			GetSmokeData()
//Description:																
//Parameters:                        										
//Return:   																
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void GetSmokeData(SmokeTag *pSmokeTagObj)
{
	#ifdef SUPPORT_SmokeTempCompensation
	uint32_t Value = 0;
	#endif
	uint32_t DiffData = 0;

	SMOKE_OP_TURN_ON();		
	AdcConfig(ADC_SamplingTime_4Cycles);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ChannelCmd(ADC1, ADC_Channel_23, ENABLE);
	Delay(100);

#ifdef SUPPORT_MATCH_SAMPLE_TIMING
	GPIO_Init(GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow);
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
#endif
	pSmokeTagObj->SmokeDarkValue = GetAdcSample(); 
#ifdef SUPPORT_MATCH_SAMPLE_TIMING
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
#endif

	SMOKE_IR_TURN_ON();   
	Delay(70);			
#ifdef SUPPORT_MATCH_SAMPLE_TIMING
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
#endif
	pSmokeTagObj->SmokeLightValue = GetAdcSample();   	
#ifdef SUPPORT_MATCH_SAMPLE_TIMING
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
#endif

	SMOKE_OP_TURN_OFF();
	SMOKE_IR_TURN_OFF();
	ADC_ChannelCmd(ADC1, ADC_Channel_23, DISABLE);
	ADC_Cmd(ADC1, DISABLE);

	//ADC换算
	pSmokeTagObj->SmokeDarkValue = AdcConvertTomVoltage(pSmokeTagObj->SmokeDarkValue);
	pSmokeTagObj->SmokeLightValue = AdcConvertTomVoltage(pSmokeTagObj->SmokeLightValue);
	
	if(pSmokeTagObj->SmokeDarkValue > pSmokeTagObj->SmokeLightValue)
	{
		pSmokeTagObj->SmokeDiffValue = 0;
	}
	else
	{
		DiffData = pSmokeTagObj->SmokeLightValue - pSmokeTagObj->SmokeDarkValue;
		//温度补偿
		#ifdef SUPPORT_SmokeTempCompensation
		Value = (uint32_t)DiffData*TempRate[SensorGetTemperature()];
		DiffData = Value/1000; 
		#endif
		
		pSmokeTagObj->SmokeDiffValue = DiffData;
	}
	#ifdef Debug_Test	
		printflog("D:%d, DVol:", pSmokeTagObj->SmokeDarkValue);
		PrintVol(pSmokeTagObj->SmokeDarkValue);
		printflog("   ");
	  	printflog("L:%d, LVol:",pSmokeTagObj->SmokeLightValue);       
	  	PrintVol(pSmokeTagObj->SmokeLightValue);
	  	printflog("   ");
	  	printflog("L-D:%d, D_LVol:",pSmokeTagObj->SmokeDiffValue);       
	  	PrintVol(pSmokeTagObj->SmokeDiffValue);
	  	printflog("\n");
	  	printflog("Sensortive:%d ",pSmokeTagObj->SmokeSensitiveValue);
	  	PrintVol(pSmokeTagObj->SmokeSensitiveValue);
	  	printflog("\n");
	#endif
}



/**********************************************************************/
//Name:			SmokerDustCompensate()
//Description:																
//Parameters:                        										
//Return:   																
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void SmokerDustCompensate(SmokeTag *pSmokeTagObj)
{
	uint16_t CailDiffValue = SmokeTagObj.SmokeSensitiveValue - SMOKE_ALARM_LEVEL;
	if(((CailDiffValue - Dust_Test_Value) < pSmokeTagObj->SmokeDiffValue) && 
								(pSmokeTagObj->SmokeDiffValue < CailDiffValue))
	{
		if(pSmokeTagObj->SmokeDiffValue < (0xFFF - SMOKE_ALARM_LEVEL))
		{
			SmokeTagObj.SmokeSensitiveValue = pSmokeTagObj->SmokeDiffValue + SMOKE_ALARM_LEVEL;
		}
	}
	else if((pSmokeTagObj->SmokeDiffValue > CailDiffValue) &&
				(pSmokeTagObj->SmokeDiffValue < (CailDiffValue + Dust_Test_Value)))
	{
		if(pSmokeTagObj->SmokeDiffValue < (0xFFF - SMOKE_ALARM_LEVEL))
		{
			SmokeTagObj.SmokeSensitiveValue = pSmokeTagObj->SmokeDiffValue + SMOKE_ALARM_LEVEL;
		}
	}
}

/**********************************************************************/
//Description:    PollingCheckSmoke()                
//Parameters:                                          
//Return:                                 
//Date:           
//Author:    quanwu.xu  
/**********************************************************************/
static void PollingCheckSmoke(TmrHdl Handle, eTmrCmd eCmd)
{
  Handle = Handle;  

  switch(eCmd)
  {
    case eInsertTimer:
      SetSysSleepFlag(eActiveFlagSmoke);
    break;
      
    case eTimeOut:
     	CheckSmokerAlarm(FALSE,FALSE);  
    break;

    case eDelTimer:
      ResetSysSleepFlag(eActiveFlagSmoke);
    break;

    default:
      break;
  }  
}

/**********************************************************************/
//Name:			CheckSmokerAlarm()
//Description:																
//Parameters:                        										
//Return:   																
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
bool CheckSmokerAlarm(bool KeyTestFlag,bool QuicklyFlag)
{
	static uint8_t WriteWDTFlag = 0;
	bool ErrRes = FALSE;
	static uint8_t SmokeAlarmTimeCnt = 0;
	static uint8_t SmokeCancelAlarmTimeCnt = 0;
	static uint8_t SmokeDevErrCnt = 0;
	static uint16_t SmokeDustCompensateCnt = 0;
	int16_t SomkeDiff = 0;
	

	GetSmokeData(&SmokeTagObj);

	SomkeDiff = SmokeTagObj.SmokeLightValue - SmokeTagObj.SmokeDarkValue;
	
	if(SmokeTagObj.SmokeDiffValue >=(SmokeTagObj.SmokeSensitiveValue-(SMOKE_ALARM_LEVEL>>1)))
	{
		if((WriteWDTFlag==0)&&(QuicklyFlag))
		{
			 TmrInsertTimer(GetTmrHd(), TMR_PERIOD(3000), PollingCheckSmoke, NULL);
			WriteWDTFlag=1;
		}
	}else
	{
		if(WriteWDTFlag==1)
		{
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingCheckSmoke));
			WriteWDTFlag=0;
		}
		SmokeAlarmTimeCnt = 0;
	}
				
	if(SmokeTagObj.SmokeState == eSmokeState_Alarm)
	{
		if(SmokeTagObj.SmokeDiffValue < SmokeTagObj.SmokeSensitiveValue)
		{
			if(SmokeCancelAlarmTimeCnt++ > 10)
			{
				SmokeTagObj.SmokeState = eSmokeState_Null;
				SmokeCancelAlarmTimeCnt = 0;
				SmokeAlarmTimeCnt = 0;
				SmokeEvent.sig = Q_eSmokeEventCancelAlarm;
				SystemPostFifo(&SmokeEvent);
			}
		}
		else
		{
			SmokeCancelAlarmTimeCnt = 0;
			SmokeAlarmTimeCnt = 0;
		}
	}
	else if(SmokeTagObj.SmokeDiffValue > SmokeTagObj.SmokeSensitiveValue)
	{
		if(SmokeTagObj.SmokeState != eSmokeState_Mute)		//smoke != mute self
		{	
			SmokeAlarmTimeCnt++;	
			if(SmokeAlarmTimeCnt > 2)
			{
				TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingCheckSmoke));
				WriteWDTFlag=0;
				SmokeTagObj.SmokeState = eSmokeState_Alarm;//*
				SmokeAlarmTimeCnt = 0;
				SmokeCancelAlarmTimeCnt = 0;
				SmokeEvent.sig = Q_eSmokeEventAlarm;
				SystemPostFifo(&SmokeEvent);
				
			}
		}
	}
	else if((SmokeTagObj.SmokeDarkValue > 1700) ||
									(SomkeDiff < LIGHT_DART_DIFF_ERR))
	{
		if(KeyTestFlag == TRUE)
		{
			ErrRes = TRUE;
			SmokeDevErrCnt = 0;
			SmokeTagObj.SmokeState = eSmokeState_DevErr;
			SmokeEvent.sig = eSystemEventSmokeErr;//*
		}
		else if(++SmokeDevErrCnt > 3)
		{
			if(SmokeTagObj.SmokeState != eSmokeState_DevErr)
			{
				SmokeTagObj.SmokeState = eSmokeState_DevErr;
				SmokeDevErrCnt = 0;
				SmokeEvent.sig = eSystemEventSmokeErr;
				SystemPostFifo(&SmokeEvent);
			}		
		}
	}
	else if(SmokeTagObj.SmokeState == eSmokeState_DevErr)
	{	
		SmokeDevErrCnt = 0;
		SmokeTagObj.SmokeState = eSmokeState_Null;
		SmokeEvent.sig = Q_eSmokeEventDevErrRecovery;
		SystemPostFifo(&SmokeEvent);
	}
	else if(SmokeDustCompensateCnt++ > 3600)			//10H
	{
		SmokeDustCompensateCnt = 0;
		SmokerDustCompensate(&SmokeTagObj);
	}

	return ErrRes;
}


/**********************************************************************/
//Description:		SmokeDevErr()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void SoundAndLightFunc(eDevErrType Type)
{
	uint8_t i = 0;

	if(Type == eDevErrType_Null)
	{
		G_LED_TURN_ON();
		DelayMs(5);
		G_LED_TURN_OFF();
	}
	else
	{
		for(i=0; i<(uint8_t)Type; i++)
		{
			Y_LED_TURN_ON();
			BEEP_TURN_ON();
			DelayMs(50);
			Y_LED_TURN_OFF();
			BEEP_TURN_OFF();
			DelayMs(100);
		}
	}
}

/******************************************************************************
**Description:				check light and dark voltag value every 2 senconds													
** Parameters:                        												
**     Return:   							none										
*******************************************************************************/

static void PollingSmokeCalibration(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t CailTimeCnt = 0;
	static uint16_t TotalDiffValue = 0;
	uint16_t SmokerSensortive = 0;
	switch(eCmd)
	{		
		case eTimeOut:
			Y_LED_TURN_ON();
					
			if(0==SensorTemperatureCheck())
			{
				printf("Temp Err0\n");
				SmokeEvent.sig = Q_eSmokeEventCailFail;
				SystemPostFifo(&SmokeEvent);
			}
			GetSmokeData(&SmokeTagObj);
			printf("T=%d\n", (SensorGetTemperature()-20));
			printf("D=%d mv, L=%d mv, L-D=%d mv\n", SmokeTagObj.SmokeDarkValue,
							SmokeTagObj.SmokeLightValue, SmokeTagObj.SmokeDiffValue);
			if((DARK_AVERAGE_MAX < SmokeTagObj.SmokeDarkValue) || 
				(SmokeTagObj.SmokeLightValue > LIGHT_AVERAGE_MAX) || (SmokeTagObj.SmokeDiffValue < LIGHT_DARK_DIFF))
			{
				printf("Sensor Err\n");
				SmokeEvent.sig = Q_eSmokeEventCailFail;
				SystemPostFifo(&SmokeEvent);
			}
			else
			{

				TotalDiffValue += SmokeTagObj.SmokeDiffValue;
				if(++CailTimeCnt >= 16)
				{
					TotalDiffValue >>= 4;
					SmokerSensortive = TotalDiffValue + SMOKE_ALARM_LEVEL;
					SmokeTagObj.SmokeSensitiveValue = SmokerSensortive;

					FLASH_Unlock(FLASH_MemType_Data);
					FLASH_ProgramByte(FLASH_DATA_EEPROM_ADDR_CALI_FLAG, CALI_FLAG_VALUE);
					
					
					FLASH_ProgramByte(FLASH_DATA_EEPROM_ADDR_SENSORTIV_DATA_MSB,
																		SmokerSensortive>>8);
					
					FLASH_ProgramByte(FLASH_DATA_EEPROM_ADDR_SENSORTIV_DATA_LSB,
																		SmokerSensortive&0xff);
					
					//FLASH_ProgramByte(FLASH_DATA_EEPROM_ADDR_TEMPTURE,	SmokeTagObj.TempValue);
					
					FLASH_Lock(FLASH_MemType_Data); 	
					SmokeEvent.sig = Q_eSmokeEventCailSuccess;
					SystemPostFifo(&SmokeEvent);					
					TmrDeleteTimer(Handle);
					ResetSysSleepFlag(eActiveFlagSmoke);
					BeepPollingHandle(100);
					printf("Cail Success  %d mv\n", SmokeTagObj.SmokeSensitiveValue);
				}
			}
			Y_LED_TURN_OFF();	
			break;
			
		default:
			break;
	}
}

/******************************************************************************
**Description:				smoke start calibration index														
** Parameters:                        	none											
**     Return:   						none											
*******************************************************************************/

static void SomkeCalibrationStart(void)
{
	LedPollingHandle(LED_TYPE_RED, 50);
	BeepPollingHandle(50);
	//定时器PollingSmokeCalibration 16次计�?* 2s执行1�?
	TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_CALIBRATION_TIME), PollingSmokeCalibration, NULL);
    SetSysSleepFlag(eActiveFlagSmoke);
}

static void PollingSmokeCalibrationOK(TmrHdl Handle, eTmrCmd eCmd)
{
	switch(eCmd)
	{
		case eTimeOut:
			{
				LedPollingHandle(LED_TYPE_GREEN, 50);
				//SmokeGetLdData(SMOKE_IR_CTR_ON_TEST_OFF);
			}
			break;
			
		default:
			break;
	}
}


/**********************************************************************/
//Description:	SysSmokeCailModeHandleEvent()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void SysSmokeCailModeHandleEvent(SystemApiTag  *me, QEvent *e)
{


	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		
			#ifdef SUPPORT_LCD_FUNCTION	
			LCD_Control(eLcdON);
			#endif
			printflog("%s (in)\n", __FUNCTION__);
			if(FLASH_ReadByte(FLASH_DATA_EEPROM_ADDR_CALI_FLAG) == CALI_FLAG_VALUE)
			{				
				LoadSmokerData();
				SmokeEvent.sig = eSystemEvenHadCailSmoke;
				SystemPostFifo(&SmokeEvent);
				
			}
			else
			{				
				SmokeEvent.sig = eSystemEvenCalibrationSmoke;
				SystemPostFifo(&SmokeEvent);
			}
			break;
			
		case Q_EXIT_SIG:
			printflog("%s (out)\n", __FUNCTION__);
			break;
		
		case eSystemEvenCalibrationSmoke://smoke校机
			
			if(FLASH_ReadByte(FLASH_DATA_EEPROM_ADDR_CALI_FLAG) == CALI_FLAG_VALUE)
			{	
				Q_TRAN(SystemStartHandle);
				#ifdef SUPPORT_SMOKE_DEBUG
				printf("%s ,%d\n",__FUNCTION__,__LINE__);
				#endif
			}
			else
			{	
				if (CheckBatteryLevel() >= eBatteryLevelThree)
				{	
					SomkeCalibrationStart();
					#ifdef SUPPORT_LCD_FUNCTION
					TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_CALIBRATION_LCD_TIME), PollingSmokeCalibrationLcdDisplay, NULL);
					SetSysSleepFlag(eActiveFlagLcd);
					#endif
				}
				else
				{
					printflog("bat err\n");
					SmokeEvent.sig = Q_eSmokeEventCailFail;
					SystemPostFifo(&SmokeEvent);
				}	
				#ifdef SUPPORT_SMOKE_DEBUG
				printf("%s ,%d\n",__FUNCTION__,__LINE__);
				#endif
			}


		case Q_eSystemEventWakeup:
		
			break;

		case Q_eSmokeEventCailFail:
			while(1)
			{
				IWDG_ReloadCounter();
				R_LED_TURN_ON();
				DelayMs(50);
				R_LED_TURN_OFF();
				DelayMs(250);
			}
			break;

		case Q_eSmokeEventCailSuccess:
			#ifdef SUPPORT_LCD_FUNCTION	
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeCalibrationLcdDisplay));
			#endif									
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(2000), PollingSmokeCalibrationOK, NULL);
		
			break;
		case eSystemEvenHadCailSmoke:
				
			#ifdef SUPPORT_CO_FUNCTION
				Q_TRAN(SysCoCailModeHandleEvent);    // init over ,tran to factory mode
			#else
				Q_TRAN(&SysIdleModeHandleEvent);
			#endif
		
			break;

		default:
			break;
	}
}


//dzs
static void PollingSmokeTest_LCD_Display(TmrHdl Handle, eTmrCmd eCmd)
{	
	switch(eCmd)
	{
		case eInsertTimer:
			#ifdef SUPPORT_LCD_FUNCTION
			BatteryLcdDisplay();
			LCD_DsiplayCoFlg(eLcdON);
			LCD_DisplayNum(0);
			#endif
			break;
	
		case eTimeOut:
			TmrDeleteTimer(Handle);
			break;
			
		case eDelTimer:
			break;	
			
		default:
			break;
	}
}

//dzs
static void PollingSmokeTest_LCDDisplay(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t i = 0;
	static uint8_t j = 0;
	
	switch(eCmd)
	{
		case eInsertTimer:
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_Clear();//	
			#endif
			break;
	
		case eTimeOut:
			if(i<16)
			{
				if(j%3==0)
				{
					#ifdef SUPPORT_LCD_FUNCTION
					LCD_Clear();
					#endif
				}
				if(i%4==3)
				{

				}else
				{
					#ifdef SUPPORT_LCD_FUNCTION
					LCD_SystemStartAnimation(j%3);//
					//printf("2PollingSmokeTest_LCDDisplay = %d\n",j);
					#endif
					j++;
				}
				++i;
			}
			else
			{
				i= 0;
				j= 0;
				#ifdef SUPPORT_LCD_FUNCTION
				LCD_Clear();//
				#endif
				TmrDeleteTimer(Handle);
				TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingSmokeTest_LCD_Display, NULL);
			}
			break;
			
		case eDelTimer:
			break;	
			
		default:
			break;
	}
}

//dzs
static void PollingSmokeTestLCDDisplayDelay(TmrHdl Handle, eTmrCmd eCmd)
{
	switch(eCmd)
	{
		case eTimeOut:
			TmrDeleteTimer(Handle);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_TEST_LCD_TIME), PollingSmokeTest_LCDDisplay, NULL);
			break;
			
		case eDelTimer:
			break;	
			
		default:
			break;
	}
}

/******************************************************************************
**Description:				smoke beep index 														
** Parameters:                  timer handle and command      												
**     Return:   				none													
*******************************************************************************/

static void PollingSmokeBeep(TmrHdl Handle, eTmrCmd eCmd)
{
	uint32_t Time = (uint32_t)TmrGetCtx(Handle);
	static uint8_t i = 0;
	
	switch(eCmd)
	{	
		case eTimeOut:
			if(i++<3)
			{
				LedPollingHandle(LED_TYPE_RED,Time);
				BeepPollingHandle(500);
				
			}else
			{
				i = 0;
				ResetSysSleepFlag(eActiveFlagSmokeBeep);
				TmrDeleteTimer(Handle);	
				SmokeEvent.sig = eSystemEventSmokeBeepOK;
				SystemPostFifo(&SmokeEvent);	
			}
			break;
		case eDelTimer:
			i = 0;
			break;	
		default:
			break;
	}
}


/******************************************************************************
**Description:					for UL beep index wave													
** Parameters:                       timer handle and command 												
**     Return:   						none											
*******************************************************************************/
static void PollingSmokeBeepDelay(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;

	switch(eCmd)
	{
		case eInsertTimer:
			break;
	
		case eTimeOut:	
			ResetSysSleepFlag(eActiveFlagSmokeBeepDelay);
			TmrDeleteTimer(Handle);				
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_BEEP_TIME), PollingSmokeBeep, ((void*)500));
			SetSysSleepFlag(eActiveFlagSmokeBeep); 
			break;
			
		case eDelTimer:
			break;	
			
		default:
			break;
	}
}


//dzs
static void PollingSmokeTestLCDDisplay(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eInsertTimer:
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_Clear();//	
			#endif
			break;
	
		case eTimeOut:
			if (i<3)
			{
				#ifdef SUPPORT_LCD_FUNCTION
				LCD_SystemStartAnimation(i%3);//
				//printf("PollingSmokeTestLCDDisplay = %d\n",i);
				#endif
				++i;
			}
			else
			{
				i= 0;
				TmrDeleteTimer(Handle);
				#ifdef SUPPORT_LCD_FUNCTION
				LCD_Clear();//	
				TmrInsertTimer(GetTmrHd(), TMR_PERIOD(500), PollingSmokeTestLCDDisplayDelay, NULL);
				#endif
				TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_KEY_TEST_DELAY_TIME), PollingSmokeBeepDelay, NULL);
				//SystemSetActiveFlag(eActiveFlagSmokeBeepDelay);
				SetSysSleepFlag(eActiveFlagSmokeBeepDelay);
			}
			break;
			
		case eDelTimer:
			break;	
			
		default:
			break;
	}
}


/******************************************************************************
**Description:					smoke key test index													
** Parameters:                       none 												
**     Return:   						none											
*******************************************************************************/

void SmokeKeyTest(void)
{	
	TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_BEEP_TIME), PollingSmokeBeep, ((void*)500));
	SetSysSleepFlag(eActiveFlagSmokeBeep);	
	

}




/******************************************************************************
**Description:					key test index delay													
** Parameters:                        none												
**     Return:   						none											
*******************************************************************************/

void SmokeKeyTestDelay(void)
{	
#if 0
	if(CheckSmokerAlarm(TRUE,FALSE) == TRUE)
	{
		SmokeEvent.sig = eSystemEventSmokeErr;
		SystemPostFifo(&SmokeEvent);
	}
	else
	{
		#ifdef SUPPORT_ESCAPE_LED_FUNCTION
			LedPollingHandle(LED_TYPE_ESCAPE,50);
		#else
			LedPollingHandle(LED_TYPE_RED,50);
		#endif
		BeepPollingHandle(50);
		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_TEST_LCD_TIME), PollingSmokeTestLCDDisplay, NULL);
	}
#endif
		#ifdef SUPPORT_ESCAPE_LED_FUNCTION
			LedPollingHandle(LED_TYPE_ESCAPE,50);
		#else
			LedPollingHandle(LED_TYPE_RED,50);
		#endif
		BeepPollingHandle(50);
		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_TEST_LCD_TIME), PollingSmokeTestLCDDisplay, NULL);
}


/**********************************************************************/
//Description:	SysSmokeTestModeEvent()								
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void SysSmokeTestModeEvent(SystemApiTag  *me,QEvent *e)
{
	static TmrHdl TestTimeoutHeader = NULL;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			printflog("%s (in)\n", __FUNCTION__);
			SmokeTagObj.SmokeState = eSmokeState_Test;
			SetSysSleepFlag(eSysActiveFlag_Test);
			
			if(CheckSmokerAlarm(TRUE,FALSE) == TRUE)
			{
				SmokeEvent.sig = eSystemEventSmokeErr;
				SystemPostFifo(&SmokeEvent);
			}
			else
			{
				TestTimeoutHeader = TmrInsertTimer(GetTmrHd(),
					TMR_PERIOD(500), PollingTestTimeout, NULL);
			}
			break;
			
		case Q_EXIT_SIG:
			printflog("%s (out)\n", __FUNCTION__);
			TmrDeleteTimer(TestTimeoutHeader);
			ResetSysSleepFlag(eSysActiveFlag_Test);
			SmokeTagObj.SmokeState = eSmokeState_Null;
			break;

		case Q_eSmokeEventTestFinished:
			Q_TRAN(&SysIdleModeHandleEvent);
			break;
		
		case eSystemEventSmokeErr:
			Q_TRAN(&SysSmokeDevErrModeHandleEvent);
			break;

		default:
			break;
	}
}


/******************************************************************************
**Description:					smoke error index 2 time													
** Parameters:                        timer handle and command												
**     Return:   						none											
*******************************************************************************/

static void PollingSmokeErrIndex(TmrHdl Handle, eTmrCmd eCmd)
{
	static uint8_t Num = 0;
	switch(eCmd)
	{
		case eTimeOut:
			{
				
				Num++;
				LedPollingHandle(LED_TYPE_YELLOW, 50);
				BeepPollingHandle(50);
				if(Num>=2)
				{
					TmrDeleteTimer(Handle); 
					ResetSysSleepFlag(eActiveFlagDevErr);
		
					#ifdef SUPPORT_SMOKE_DEBUG			
					printf("%s ,%d\n",__FUNCTION__,__LINE__);
					#endif
				}else
				{
					#ifdef SUPPORT_SMOKE_DEBUG			
					printf("%s ,%d\n",__FUNCTION__,__LINE__);
					#endif
				}
				
			}
			break;
		case eDelTimer:
			{
				Num = 0;
			}
			break;
		default:
			break;
	}
}
/******************************************************************************
**Description:				smoke index cycle														
** Parameters:                    timer handle and command    												
**     Return:   				none													
*******************************************************************************/

static void PollingSmokeErr(TmrHdl Handle, eTmrCmd eCmd)
{
	switch(eCmd)
	{
		case eTimeOut:
 			{
				//printf("55s poll\n");
				TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_ERR_INDEX_TIME), PollingSmokeErrIndex, NULL);
				SetSysSleepFlag(eActiveFlagDevErr);
			}
			break;
			
		default:
			break;
	}
}
void SensorErrUpdata(void)
{
	#ifdef SUPPORT_WIFI_NET_FUNCTION
	#ifdef WiFiUpdataNew
	SetWiFiUpdata(DPID_FAULT,sensor_fault,0xff,0,0xff,0);
	WiFiOpenCheck();
	#else
	Get_DpId_Data(DPID_FAULT,sensor_fault);
	#endif
	#endif
}

/******************************************************************************
**Description:				SystemSmokeError														
** Parameters:           me :AO     e : event         												
**     Return:   			none														
*******************************************************************************/

void SystemSmokeError(SystemApiTag  *me,QEvent *e)
{
	switch (e->sig)
	{
		case Q_eSystemEventWakeup:		

			CheckSmokerAlarm(FALSE, TRUE);
			#ifdef SUPPORT_CO_FUNCTION 
			COSelfCheck();
			#endif
	        SysTimeUpdate();
			SystemPollingIdle();
			#ifdef Support_keytesterror_debug
			printf("eSystemEventWakeup, %s, %d\r\n",__FUNCTION__,__LINE__);
			#endif
			break;
			
		case Q_ENTRY_SIG:
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_DsiplayStr(eStr_Err,0,0);//"Err"	
			#endif 
			
			#ifdef SUPPORT_NET_FUNCTION
				if(SystemGetNetMode()==ePairedHub)//hub
				{
					UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgFault, DevSmokeSenorFault, 0);
				}
			#endif

			#ifdef SUPPORT_WIFI_NET_FUNCTION1
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_FAULT,sensor_fault,0xff,0,0xff,0);
			WiFiOpenCheck();
			#else
			Get_DpId_Data(DPID_FAULT,sensor_fault);
			#endif
			#endif
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_ERR_INDEX_TIME), PollingSmokeErrIndex, NULL);			
			SetSysSleepFlag(eActiveFlagDevErr);
			TmrInsertTimer(GetTmrIdleHd(), TMR_IDLE_PERIOD(SMOKE_ERR_TIME), PollingSmokeErr, NULL);
			#ifdef Debug_Test
			printf("Event:smoke Err\n");
			#endif
			break;
			
		case Q_EXIT_SIG:
			#ifdef SUPPORT_LCD_FUNCTION
				LCDStandyDisplay();
			#endif
			
			#ifdef SUPPORT_NET_FUNCTION
			if(SystemGetNetMode()==ePairedHub)//hub
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgFault, DevCancelFault, 0);
			}
			#endif	
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			SetWiFiUpdata(DPID_FAULT,faultCancel,0xFF,0,0xff,0);
			WiFiOpenCheck();
			#endif
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrIdleHd(),PollingSmokeErr));
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingSmokeErrIndex));
			ResetSysSleepFlag(eActiveFlagDevErr);
			#ifdef Debug_Test
			printf("%s out,%d\n",__FUNCTION__,__LINE__);
			#endif
			break;
		
		case Q_eSmokeEventCancelAlarm:	
			Q_TRAN(SysIdleModeHandleEvent);
			break;
        case Q_eSmokeEventDevErrRecovery:
			#ifdef SUPPORT_LCD_FUNCTION
				LCDStandyDisplay();
			#endif
			Q_TRAN(SysIdleModeHandleEvent);
			break;
		#ifdef SUPPORT_CO_FUNCTION
		case eSystemEventCOAlarm:

			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_CO_STATE,COAlarm,DPID_CO_VALUE,GetCOPpm(),0xff,0);
			WiFiOpenCheck();
			#else
			Get_DpId_Data(DPID_MUFFLING,MuteStateOff);//Cancel Mute & Cancel Alarm
			#endif
			#endif
			Q_TRAN(SystemCOAlarm);
			break;
		#endif
#if 1
		case Q_eKeyEventKey1Clik:
			{
				Q_TRAN(SystemKeyTestHandle);	
			}
			break;
#endif
		default:
			#ifdef SUPPORT_SMOKE_DEBUG
			printf("%s ,sig %d\n",__FUNCTION__,e->sig);
			#endif
			break;
	}
}
//dzs
static void Smoke_PollingCancelAlarm(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;
    static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut:
			if (i<5)
			{
				LedPollingHandle(LED_TYPE_GREEN, 500);			
				i++;
			}
			else
			{
				i = 0;
				#ifdef SUPPORT_LCD_FUNCTION
				LCD_BackLightControl(eLcdBackLightOFF);
				#endif
				TmrDeleteTimer(Handle);
				ResetSysSleepFlag(eActiveFlagCancelAlarmLed);
				SmokeEvent.sig = eSystemEventSmokeAlarmCancelTimeout;
				SystemPostFifo(&SmokeEvent);
			}    
			break;
			
		case eDelTimer:
			i = 0;	
			break;
		
		default:
			break;
	}
}

/******************************************************************************
**Description:				smoke beep index 														
** Parameters:                  timer handle and command      												
**     Return:   				none													
*******************************************************************************/

static void PollingSmokeLed(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;
	static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut:
			if(i++<3)
			{
				LedPollingHandle(LED_TYPE_RED,500);
			}else
			{
				i = 0;
				ResetSysSleepFlag(eActiveFlagSmokeBeep);
				TmrDeleteTimer(Handle);				
			}
			break;
		case eDelTimer:
			i = 0;
			break;	
		default:
			break;
	}
}

/******************************************************************************
**Description:				smoke mute check time out														
** Parameters:            timer handle and command         												
**     Return:   			none														
*******************************************************************************/

static void PollingSmokeMute(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;
	static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut:	
			i++;
			if(i>=SMOKE_MUTE_TIME_NUM)
			{
				i = 0;
				TmrDeleteTimer(Handle);
				SmokeEvent.sig = eSystemEventSmokeMuteTimeout;
				SystemPostFifo(&SmokeEvent);
			}else
			{
				//LedPollingHandle(LED_TYPE_RED,10);
				TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_BEEP_TIME), PollingSmokeLed, NULL);
				SetSysSleepFlag(eActiveFlagSmokeBeep); 
				//SmokeCheckEvt();
				CheckSmokerAlarm(FALSE, FALSE);
				
				#ifdef SUPPORT_WIFI_NET_FUNCTION1
				if(SmokeTagObj.SmokeDiffValue <=(SmokeTagObj.SmokeSensitiveValue-(SMOKE_ALARM_LEVEL>>1)) )
				{
					#ifdef WiFiUpdataNew
					SetWiFiUpdata(DPID_SMOKE_SENSOR_STATE,SmokeNULL,0xff,0,0xff,0);
					WiFiOpenCheck();
					#else
					Get_DpId_Data(DPID_SMOKE_SENSOR_STATE,SmokeNULL);
					#endif
				}
				#endif
			}
			break;

			
		case eDelTimer:
			i = 0;
			break;
		
		default:
			break;
	}
}


static void SMOKE_PollingCloseLcdBackLightDelay(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;

	switch(eCmd)
	{
		case eTimeOut:
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_BackLightControl(eLcdBackLightOFF);//
			#endif
			TmrDeleteTimer(Handle);				
			ResetSysSleepFlag(eActiveFlagCloseLcdBackLight);
			break;
			
		case eDelTimer:
			break;	
			
		default:
			break;
	}
}
//dzs
static void Smoke_PollingMuteAlarm(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;
    static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut:
			if (i<3)
			{
				LedPollingHandle(LED_TYPE_GREEN, 500);			
				i++;
			}
			else
			{
				i = 0;
				#ifdef SUPPORT_LCD_FUNCTION
				LCD_BackLightControl(eLcdBackLightOFF);
				#endif
				TmrDeleteTimer(Handle);
				ResetSysSleepFlag(eActiveFlagSmokeMuteAlarmLed);
				SmokeEvent.sig = eSystemEventSmokeMuteExitTimeout;
				SystemPostFifo(&SmokeEvent);
			}    
			break;
			
		case eDelTimer:
			i = 0;	
			break;
		
		default:
			break;
	}
}

#ifdef SUPPORT_NET_FUNCTION

static void Net_PollingSmokeBeep(TmrHdl Handle, eTmrCmd eCmd)
{
	uint32_t Time = (uint32_t)TmrGetCtx(Handle);
	static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut:
			if(i++<3)
			{
				LedPollingHandle(LED_TYPE_RED,Time);
				BeepPollingHandle(500);
				SetSysSleepFlag(eActiveFlagNetKeyTestGREENLED);
        		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(510), PollingLedGreen, ((void*)400));//
			}else
			{
				i = 0;
				ResetSysSleepFlag(eActiveFlagSmokeBeep);
				TmrDeleteTimer(Handle);	
				SmokeEvent.sig = eSystemEventSmokeBeepOK;
				SystemPostFifo(&SmokeEvent);				
			}
			break;
		case eDelTimer:
			i = 0;
			break;	
		default:
			break;
	}
}


/******************************************************************************
**Description:					smoke alarm state handle 													
** Parameters:                   object and event      												
**     Return:   					none												
*******************************************************************************/

void NetSmokeAlarm(SystemApiTag  *me,QEvent *e)
{
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_BEEP_TIME), Net_PollingSmokeBeep, ((void*)500));
			SetSysSleepFlag(eActiveFlagSmokeBeep); 
			#ifdef DEBUG_NET
			printf("%s in\n",__FUNCTION__);
			#endif
			break;
			
		case Q_EXIT_SIG:
			
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), Net_PollingSmokeBeep));
			ResetSysSleepFlag(eActiveFlagSmokeBeep);
			#ifdef DEBUG_NET
			printf("%s out\n",__FUNCTION__);
			#endif
			break;

		case Q_eKeyEventKey1Clik:	
			#ifdef SUPPORT_NET_FUNCTION
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0, 0);
		case eNETEventMute:
			#endif
			Q_TRAN(SysIdleModeHandleEvent);
		break;

		break;
		
		case eSystemEventSmokeBeepOK:
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_BEEP_TIME), Net_PollingSmokeBeep, ((void*)500));
			SetSysSleepFlag(eActiveFlagSmokeBeep); 	
			#ifdef SUPPORT_SMOKE_FUNCTION
			CheckSmokerAlarm(FALSE, FALSE);
			#endif
			break;
			
		case eNETEventCancleAlarm:
			Q_TRAN(SysIdleModeHandleEvent);
			break;
			
		case Q_eSmokeEventAlarm:
			#ifdef SUPPORT_SMOKE_FUNCTION
			Q_TRAN(SysSmokeAlarmModeEvent);
			#endif
			break;
		
		default:
			#ifdef SUPPORT_SMOKE_DEBUG
			printf("sig %d\n",e->sig);
			#endif
			break;
	}
}

#endif



/******************************************************************************
**Description:				smoke mute state handle														
** Parameters:                 object and event       												
**     Return:   					none												
*******************************************************************************/

static void SystemSmokeMute(SystemApiTag  *me,QEvent *e)
{
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_BackLightControl(eLcdBackLightON);
			#endif
			#ifdef Debug_Test
			printf("%s in,%d\n",__FUNCTION__,__LINE__);
			#endif
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_MUTE_TIME), PollingSmokeMute, NULL);
			SetSysSleepFlag(eActiveFlagSmokeMute);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CLOSE_LCD_BACKLIGHTTIME), SMOKE_PollingCloseLcdBackLightDelay, NULL);//////
			SetSysSleepFlag(eActiveFlagCloseLcdBackLight);//////
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			
			#else
			Get_DpId_Data(DPID_MUFFLING,MuteStateOn);
			#endif
			#endif
			break;
			
		case Q_EXIT_SIG:
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeMute));
			ResetSysSleepFlag(eActiveFlagSmokeMute);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeLed));
			ResetSysSleepFlag(eActiveFlagSmokeBeep);
			SmokeTagObj.SmokeState = eSmokeState_Null;
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_MUFFLING,MuteStateOff,DPID_SMOKE_SENSOR_STATE,SmokeNULL,0xff,0);
			WiFiOpenCheck();
			#else
			Get_DpId_Data(DPID_MUFFLING,MuteStateOff);
			#endif
			#endif
			#ifdef Debug_Test
			printf("%s out,%d\n",__FUNCTION__,__LINE__);
			#endif
			break;
			
	    case eSystemEventSmokeMuteTimeout:
			#ifdef SUPPORT_NET_FUNCTION
			if(IsNetEnable())
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0, 0);
			}
			#endif
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_BackLightControl(eLcdBackLightON);
			LCD_DisplayNum(0);
			#endif
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), Smoke_PollingMuteAlarm, NULL);
			SetSysSleepFlag(eActiveFlagSmokeMuteAlarmLed);
			break;

		case eSystemEventSmokeMuteExitTimeout:
			Q_TRAN(SysIdleModeHandleEvent);
			break;

		case eSystemEventSmokeErr:
			Q_TRAN(SysIdleModeHandleEvent);
			break;
		
		#ifdef SUPPORT_NET_FUNCTION
		//app 下发取消静音
		case eNETEventCancelMute:
			if(SystemGetNetMode()==ePairedHub)
			{
				Q_TRAN(SysIdleModeHandleEvent); 
			}			 	
		break;
		
		case eNETEventSmokeAlarm:

			 Q_TRAN(NetSmokeAlarm); 	
			break;
		case eNETEventCoAlarm:

			Q_TRAN(NetCOAlarm); 	
			break;
		#endif

		default:	
			#ifdef SUPPORT_SMOKE_DEBUG
			printf("%s ,sig %d\n",__FUNCTION__,e->sig);
			#endif
			break;
	}

}




/**********************************************************************/
//Description:	SysSmokeAlarmModeEvent()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void SysSmokeAlarmModeEvent(SystemApiTag  *me,QEvent *e)
{

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			#ifdef Debug_Test
			printflog("%s (in), %d\n", __FUNCTION__, __LINE__);
			#endif
			#ifdef SUPPORT_LCD_FUNCTION
				LCD_BackLightControl(eLcdBackLightON);
				LCD_DisplayNum(0);
			#endif
			#ifdef SUPPORT_NET_FUNCTION
				if(IsNetEnable())
				{
					//smoke 报警
					UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgSmokeAlarm, 0, 0);
				}
			#endif
			
			#ifdef SUPPORT_WIFI_NET_FUNCTION1
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_SMOKE_SENSOR_STATE,SmokeIs,0xff,0,0xff,0);
			WiFiOpenCheck();
			#else
			Get_DpId_Data(DPID_SMOKE_SENSOR_STATE,SmokeIs);
			#endif
			#endif
			SmokeTagObj.SmokeState = eSmokeState_Alarm;
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_BEEP_TIME), PollingSmokeBeep, ((void*)500));
			SetSysSleepFlag(eActiveFlagSmokeBeep);
		break;
			
		case Q_EXIT_SIG:
			#ifdef Debug_Test
			printflog("%s (out), %d\n", __FUNCTION__, __LINE__);
			#endif		
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeBeep));
			ResetSysSleepFlag(eActiveFlagSmokeBeep);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), Smoke_PollingCancelAlarm));
			ResetSysSleepFlag(eActiveFlagCancelAlarmLed);
			#ifdef SUPPORT_LCD_FUNCTION
				LCD_BackLightControl(eLcdBackLightOFF);
			#endif
			SmokeTagObj.SmokeState = eSmokeState_Null;
			break;

		case Q_eKeyEventKey1Clik:
			SmokeTagObj.SmokeState = eSmokeState_Null;
			#ifdef SUPPORT_NET_FUNCTION
			if(IsNetEnable())
			{
				//smoke 静音
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgMute, 0, 0);
			}
		case eNETEventMute:
			#endif
			
		#ifdef SUPPORT_WIFI_NET_FUNCTION
			SetWiFiUpdata(DPID_MUFFLING,MuteStateOn,0xff,0,0xff,0);
			WifiAO.WifiCONNFlag=TRUE;
		case eWIFINETEventMute:
			WifiAO.WifiCMDTimeout=4;
			SmokeTagObj.SmokeState = eSmokeState_Null;
		#endif
			Q_TRAN(SystemSmokeMute);
			break;

		case Q_eSmokeEventCancelAlarm:
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeBeep));
			ResetSysSleepFlag(eActiveFlagSmokeBeep);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), Smoke_PollingCancelAlarm, NULL);
			SetSysSleepFlag(eActiveFlagCancelAlarmLed);
			#ifdef SUPPORT_NET_FUNCTION
			if(IsNetEnable())
			{
				//取消smoke报警
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0, 0);
			}
			#endif
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_SMOKE_SENSOR_STATE,SmokeNULL,0xff,0,0xff,0);
			WifiAO.WifiCONNFlag=TRUE;
			#else
			Get_DpId_Data(DPID_SMOKE_SENSOR_STATE,SmokeNULL);
			#endif
			#endif
			break;

		case eSystemEventSmokeBeepOK:		
			if(SmokeTagObj.SmokeState==eSmokeState_Null)//cancel alarm
			{
				SmokeEvent.sig = Q_eSmokeEventCancelAlarm;
				SystemPostFifo(&SmokeEvent);
			}else
			{
				CheckSmokerAlarm(FALSE,FALSE);
			}
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(SMOKE_BEEP_TIME), PollingSmokeBeep, ((void*)500));
			SetSysSleepFlag(eActiveFlagSmokeBeep); 
			#ifdef Debug_Test
			printf(" DVol:%d  mv",SmokeTagObj.SmokeDarkValue);
			printf(" LVol:%d  mv",SmokeTagObj.SmokeLightValue);
			#endif
		    
			break;

		case eSystemEventSmokeAlarmCancelTimeout:
			SmokeEvent.sig = Q_eSystemEventWakeup;
			SystemPostFifo(&SmokeEvent);
			Q_TRAN(SysIdleModeHandleEvent);
			break;
			

		case eSystemEventSmokeErr:
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingSmokeBeep));
			ResetSysSleepFlag(eActiveFlagSmokeBeep);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), Smoke_PollingCancelAlarm, NULL);
			SetSysSleepFlag(eActiveFlagCancelAlarmLed);
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			SetWiFiUpdata(DPID_FAULT,sensor_fault,0xff,0,0xff,0);
			WifiAO.WifiCONNFlag=TRUE;
			#endif
			Q_TRAN(SystemSmokeError);
			break;
		default:
			break;
	}
}

eSmokeState GetSmokeState(void)
{
	return SmokeTagObj.SmokeState;
}

void SmokeStateInit(void)
{
	 SmokeTagObj.SmokeState=eSmokeState_Null;
}


