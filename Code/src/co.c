#include "smoke.h"
#include "co.h"
#include "adc.h"
#include "pwr.h"
#include "flash.h"
#include "led.h"
#include "lcd.h"
#include "beep.h"
#include "RF.h"
#include "SceneInstance.h"
#include "WiFiUpdata.h"
#include "protocol.h"
#include "wifi.h"


#define CO_CALIBRATION_CHECK_TIME   (3000)
#define CO_ALARM_COUNT        (4)



#ifdef SUPPORT_CO_FUNCTION
#define CO_ADC_PORT				GPIOC
#define CO_ADC_PIN				GPIO_Pin_7

#define CO_TEST_PORT			GPIOE
#define CO_TEST_PIN				GPIO_Pin_6

#define CO_TEST_TURN_ON()		{GPIO_SetBits(CO_TEST_PORT, CO_TEST_PIN);}
#define CO_TEST_TURN_OFF()		{GPIO_ResetBits(CO_TEST_PORT, CO_TEST_PIN);}



#endif


//co
#define CALI_FLAG_VALUE		((uint8_t)0x5B)
#define FLASH_DATA_EEPROM_ADDR_CO_CALI_FLAG				((uint16_t)0x1010) 

#define CO_CALIBRATION_STEP0_TIME   (1000)
#define CO_CALIBRATION_STEP1_TIME   (2000)
#define CO_CALIBRATION_STEP2_TIME   (3000)
#define CO_CALIBRATION_CHECK_TIME   (3000)
#define CO_CALIBRATION_INTERVAL_TIME   (10000)
#define CO_BEEP_TIME        		(200)
#define CO_BEEPDELAY_TIME        	(4700)
#define CO_ERR_TIME        			(55)
#define CO_CALIBRATION_ERR_TIME     (10)
#define CO_MUTE_TIME        		(270000)
#define CO_MUTE_TIME_OUT        	(240000)
#define CO_ALARM_TIME_OUT       	(60000)
#define CO_CALIBRATION_DELAY_TIME   (300)//weit for stable  3s*100
#define CO_SELFCHECK_COUNT          (6)                     // 60s/SLEEP_TIME=6
#define CO_CHECK_COUNT              (3)                     // 30s/SLEEP_TIME=6
#define CO_KEY_TEST_DELAY_TIME         (1000)//MS

#define CO_CALIBRATION_COUNT        (4)

#define CO_TEST_LCD_TIME        		(450)
#define CO_CALIBRATION_STEP0_LCD_TIME   (250)
#define CO_CALIBRATION_STEP1_LCD_TIME   (500) 
#define CO_CALIBRATION_STEP2_LCD_TIME   (1000)



const uint16_t COTempOffsetRate[91]={
CO_TN_20,CO_TN_19,CO_TN_18,CO_TN_17,CO_TN_16, 
CO_TN_15,CO_TN_14,CO_TN_13,CO_TN_12,CO_TN_11,
CO_TN_10,CO_TN_9, CO_TN_8, CO_TN_7, CO_TN_6, 
CO_TN_5, CO_TN_4, CO_TN_3, CO_TN_2, CO_TN_1, 
CO_TP_0, 
CO_TP_1, CO_TP_2, CO_TP_3, CO_TP_4, CO_TP_5, 
CO_TP_6, CO_TP_7, CO_TP_8, CO_TP_9, CO_TP_10,
CO_TP_11,CO_TP_12,CO_TP_13,CO_TP_14,CO_TP_15,
CO_TP_16,CO_TP_17,CO_TP_18,CO_TP_19,CO_TP_20, 
CO_TP_21,CO_TP_22,CO_TP_23,CO_TP_24,CO_TP_25,
CO_TP_26,CO_TP_27,CO_TP_28,CO_TP_29,CO_TP_30,
CO_TP_31,CO_TP_32,CO_TP_33,CO_TP_34,CO_TP_35,
CO_TP_36,CO_TP_37,CO_TP_38,CO_TP_39,CO_TP_40,
CO_TP_41,CO_TP_42,CO_TP_43,CO_TP_44,CO_TP_45,
CO_TP_45,CO_TP_47,CO_TP_48,CO_TP_49,CO_TP_50,
CO_TP_51,CO_TP_52,CO_TP_53,CO_TP_54,CO_TP_55,
CO_TP_56,CO_TP_57,CO_TP_58,CO_TP_59,CO_TP_60,
CO_TP_61,CO_TP_62,CO_TP_63,CO_TP_64,CO_TP_65,
CO_TP_66,CO_TP_67,CO_TP_68,CO_TP_69,CO_TP_70 
};

#ifdef SUPPORT_CoTempCompensation
const uint16_t COTempSlopeRate[91]={
655,664,674,684,694,704,714,723,733,742,				// -20 - -11
752,761,771,780,789,799,808,817,826,835,				// -10 - -1
844,												  // 0
852,861,870,878,887,895,903,911,919,927,				// 1 - 10
935,943,950,958,965,972,980,987,994,1000,				// 11 - 20
1007,1013,1020,1026,1032,1038,1044,1050,1055,1060,		// 21 - 30
1066,1071,1076,1080,1085,1089,1094,1098,1101,1105,		// 31 - 40
1109,1112,1115,1118,1121,1124,1126,1128,1130,1132,		// 41 - 50
1134,1135,1136,1137,1138,1139,1139,1139,1139,1139,		// 51 - 60
1139,1139,1139,1139,1139,1139,1139,1139,1139,1139		// 61 - 70
};


#else
const uint16_t COTempSlopeRate[91]={
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,                // -20 - -11
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,                // -10 - -1
1000,                                                  // 0
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,                // 1 - 10
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,               // 11 - 20
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,      // 21 - 30
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,      // 31 - 40
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,      // 41 - 50
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,      // 51 - 60
1000,1000,1000,1000,1000,1000,1000,1000,1000,1000       // 61 - 70
};
#endif






typedef  struct COCali{
	uint8_t  COCaliState;		// 0: step 0; 1:step 1; 2:step 2;0xa5 :caliOK
	uint16_t Sample;			// adc voltage
	uint16_t COCaliGradient;	// 	k;  y = k*x + b;
	uint16_t COCaliOffset;		//	b;  y = k*x + b;
	uint16_t COCaliThresholdMin;// calibration threshold min value
	uint16_t COCaliThresholdMax;// calibration threshold max value
}COCaliTag;

struct CO{

	uint8_t State;				// CO state
    uint16_t Sample;			// CO out adc voltage
	uint16_t Ppm;   			// CO PPM 
	uint16_t PpmPeak; 			// CO PPM history peak
	uint16_t COCaliGradient;	//	k;  y = k*x + b;
	uint16_t COCaliOffset;		//  b;  y = k*x + b;
    uint16_t COAlarmThreshold0;	//	
	uint16_t COAlarmThreshold1;	//	
	uint16_t COAlarmThreshold2;	//
	uint16_t COAlarmThreshold3;	//	
	uint16_t COAlarmThreshold4;	//
	uint8_t COFaultOpenCount;	// CO OPEN ERR COUNTER  >2 ALARM
	uint8_t COFaultShortCount;	// CO SHORT ERR COUNTER >2 ALARM

};


static COTag CoObject;
static QEvent COEvt;
static COCaliTag CoCaliObject = {0};

void COInit(void)
{
	GPIO_Init(CO_ADC_PORT, CO_ADC_PIN, GPIO_Mode_In_FL_No_IT);
	GPIO_Init(CO_TEST_PORT, CO_TEST_PIN, GPIO_Mode_Out_PP_Low_Slow);
	CO_TEST_TURN_OFF();	
	
	CoObject.State = 0;
	CoObject.Sample = 0;
	CoObject.Ppm = 0;
    CoObject.COCaliGradient = FlashGetCOGradient();
	CoObject.COCaliOffset = FlashGetCOOffset();
	CoObject.COAlarmThreshold0 = 0;
	CoObject.COAlarmThreshold1 = 0;
	CoObject.COAlarmThreshold2 = 0;
	CoObject.COAlarmThreshold3 = 0;
	CoObject.COAlarmThreshold4 = 0;
    
}

uint8_t GetCOState(void)
{
	return CoObject.State;
}



uint16_t GetCOPpm(void)
{
	return CoObject.Ppm;
}

void COSetPeakData(uint16_t PpmPeakValue)
{
	CoObject.PpmPeak=PpmPeakValue;
}


uint16_t COGetPeakData(void)
{
	return CoObject.PpmPeak;
}

/*****************************************************************************
*???????????? : CoCalibrationValueInit
*???????????? : CO???????????????
*???????????? : pcali:co??????   
*???????????? : ???
*???????????? : 
*****************************************************************************/
static void CoCalibrationValueInit(COCaliTag *pcali)
{
	uint16_t COCaliThresholdMin = PPM_CARIBRATON_STEP0_THRESHOLD_MIN;
	uint16_t COCaliThresholdMax = PPM_CARIBRATON_STEP0_THRESHOLD_MAX;
	switch(pcali->COCaliState)
	{
		case 0:
		COCaliThresholdMin = PPM_CARIBRATON_STEP0_THRESHOLD_MIN;
		COCaliThresholdMax = PPM_CARIBRATON_STEP0_THRESHOLD_MAX;
		break;
		
		case 1:
		COCaliThresholdMin = PPM_CARIBRATON_STEP1_THRESHOLD_MIN+pcali->COCaliOffset;
		COCaliThresholdMax = PPM_CARIBRATON_STEP1_THRESHOLD_MAX+pcali->COCaliOffset;
		break;
		
		case 2:
		COCaliThresholdMin = pcali->COCaliOffset+(PPM_CARIBRATON_STEP2_THRESHOLD_MIN*pcali->COCaliGradient);
		COCaliThresholdMax = pcali->COCaliOffset+(PPM_CARIBRATON_STEP2_THRESHOLD_MAX*pcali->COCaliGradient);
		break;
	}
	pcali->COCaliThresholdMin = COCaliThresholdMin;
	pcali->COCaliThresholdMax = COCaliThresholdMax;

	printf("CO Cail Step = %d\n",(int)pcali->COCaliState);
	printf("COCaliThresholdMin = %d\n",COCaliThresholdMin);
	printf("COCaliThresholdMax = %d\n",COCaliThresholdMax);

}



/*****************************************************************************
*???????????? : COAdcGetSample
*???????????? : CO ADC????????????
*???????????? : pCO			???co??????
			State	???CO_NORMAL_CHECK		????????????
					  CO_SHORT_FAULT_CHECK	??????????????????
*???????????? :	CoTemp		???Co??????
*???????????? : 
*****************************************************************************/
static uint16_t COAdcGetSample(COTag *pCO,uint8_t State)
{ 
    uint16_t CoTemp=0;   
	switch (State)
	{
		case CO_NORMAL_CHECK:
			;
			break;
		case CO_SHORT_FAULT_CHECK:
			{
				CO_DETECT_SHORT_ON();       //enable fault detect
			    DelayMs(10);                //10ms  (470mv)
			    CO_DETECT_SHORT_OFF();     // disable fault detect
			}
			break;
		default:
			break;		
		
	}
	CoTemp = AdcGetChannelValue(ADC_SamplingTime_4Cycles,CO_DETECT_ADC_CHANNEL);
	CoTemp =  AdcConvertTomVoltage(CoTemp);
	pCO->Sample = CoTemp;

    return CoTemp;
}


/*****************************************************************************
*???????????? : COTempCalibrationData
*???????????? : CO????????????
*???????????? :   
*???????????? :
*???????????? : 
*****************************************************************************/
static uint16_t COTempCalibrationData(COTag *pCO)
{
	uint16_t Ret = 0;
	uint32_t COData=COAdcGetSample(pCO,CO_NORMAL_CHECK);	
	COData=COData*1000;
    Ret=COData/COTempOffsetRate[SensorGetTemperature()];
	return Ret;
}


/*****************************************************************************
*???????????? : COGetData
*???????????? : CO????????????
*???????????? :   
*???????????? :
*???????????? : 
*****************************************************************************/
static uint16_t COGetData(void)
{
	uint16_t COTemp = 0;
	COTemp = COTempCalibrationData(&CoObject);
	printf("\nco sample:%.3f(%d)\n",MVoltageConvertToVoltage(COTemp),COTemp);
	return COTemp;
}
/*****************************************************************************
*???????????? : COCalibrationStartCheck
*???????????? : CO????????????????????????
*???????????? : COData???CO??????   
*???????????? : Ret		  ????????? 1?????? 0??????
*???????????? : 
*****************************************************************************/
static uint8_t COCalibrationStartCheck(COCaliTag *pcali,uint16_t COData)
{
	uint8_t Ret = 0;
	static uint8_t NUM = 0;// for  pwr  stable
	
	if ((COData>pcali->COCaliThresholdMin)&&(COData<pcali->COCaliThresholdMax))
	{
		if(NUM>=5)//6 time
		{
			Ret = 1;
			printf("Ret : %d \n",Ret);
		}else
		{
			NUM++;
		}
	}
	else
	{
		Ret = 0;
		NUM = 0;
	}
	return Ret;
}
/*****************************************************************************
*???????????? : COCalibrationCheck
*???????????? : pcali		???CO??????
*???????????? : COData  :	CO??????   
*???????????? : ???
*???????????? : 
*****************************************************************************/
static uint8_t COCalibrationCheck(COCaliTag *pcali,uint16_t COData)
{
	uint8_t ret = 1;
	if ((COData>pcali->COCaliThresholdMin)&&(COData<pcali->COCaliThresholdMax))
	{
		COEvt.sig = eSystemEvenCalibrationCOOK;
		pcali->Sample = COData;	
		
	}
	else
	{
		COEvt.sig = eSystemEvenCalibrationCOErr;
		ret = 0;
		
	}
	SystemPostFifo(&COEvt);
	return ret;
}

/*****************************************************************************
*???????????? : PollingCOCalibration
*???????????? : Handle??????????????????
*???????????? : eCmd  :	??????   
*???????????? : ???
*???????????? : 
*****************************************************************************/
static void PollingCOCalibration(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;									

	switch(eCmd)
	{
		case eTimeOut:
			if (SensorTemperatureCheck())
			{
				static uint32_t COData = 0;
				static uint8_t i =0 ;
				if (i<(1<<CO_CALIBRATION_COUNT))
				{
					LedPollingHandle(LED_TYPE_YELLOW, 10);					
					COData += COGetData();
					i++;
					printf("co cnt = %d\n",i);
				}
				else
				{
					i = 0;
					COData = COData>>CO_CALIBRATION_COUNT;				
					COCalibrationCheck(&CoCaliObject,(uint16_t)COData);				
					COData = 0;
					TmrDeleteTimer(Handle);
					
				}
				
			}
			else
			{
				COEvt.sig = eSystemEvenCalibrationTempErr;
				SystemPostFifo(&COEvt);
				TmrDeleteTimer(Handle);
			}
			
			break;
			
		default:
			break;
	}
}


/*****************************************************************************
*???????????? : COCalibrationStart
*???????????? : CO??????
*???????????? : pcali:co??????   
*???????????? : ???
*???????????? : 
*****************************************************************************/
static void COCalibrationStart(COCaliTag *pcali)
{

	TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_CALIBRATION_INTERVAL_TIME), PollingCOCalibration, NULL);
    SetSysSleepFlag(eActiveFlagCO);
}


/*****************************************************************************
*???????????? : PollingCOCalibrationStartCheck
*???????????? : CO????????????
*???????????? : pcali:co??????   
*???????????? : ???
*???????????? : 
*****************************************************************************/
static void PollingCOCalibrationStartCheck(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;	
	uint16_t COTemp = 0;
	static uint16_t Num = 0;
	static uint16_t StartNum = 0;
	switch(eCmd)
	{
		case eTimeOut:
			
			 LedPollingHandle(LED_TYPE_YELLOW, 10);
			 COTemp = COGetData();			
			 if(COCalibrationStartCheck(&CoCaliObject, COTemp))
		 	 {	
			    if(Num>CO_CALIBRATION_DELAY_TIME)// co  ??????300s
		    	{
					COCalibrationStart(&CoCaliObject);
					TmrDeleteTimer(Handle);
					Num = 0;
		    	}else
		    	{	
					Num++;
					//StartNum = 0;
					printf("Num : %d \n",Num);
				}
		 	 }else
		 	 {
				Num = 0;
				if(StartNum>CO_CALIBRATION_DELAY_TIME)
				{
					StartNum = 0;
					COEvt.sig = eSystemEvenCalibrationCOErr;
					SystemPostFifo(&COEvt);
					TmrDeleteTimer(Handle);

				}else
				{
					StartNum++;
					printf("StartNum : %d \n",StartNum);

				}
					
			 }
		
			break;
			
		default:
			break;
	}
}


/*****************************************************************************
*???????????? : COCalibrationCheckStart
*???????????? : CO????????????
*???????????? : pcali:co??????   
*???????????? : ???
*???????????? : 
*****************************************************************************/
static void COCalibrationCheckStart(COCaliTag *pcali)
{
	uint32_t ReloadCnt = TMR_PERIOD(CO_CALIBRATION_CHECK_TIME);
	CoCalibrationValueInit(pcali);
	switch(pcali->COCaliState)
	{
		case 0:
		ReloadCnt = TMR_PERIOD(CO_CALIBRATION_STEP0_TIME);
		break;
		
		case 1:
		ReloadCnt = TMR_PERIOD(CO_CALIBRATION_STEP1_TIME);
		break;
		
		case 2:
		ReloadCnt = TMR_PERIOD(CO_CALIBRATION_STEP2_TIME);	
		break;
	}
	#ifdef SUPPORT_LCD_FUNCTION
	LCD_Clear();
	LCD_DisplayNum(pcali->COCaliState);
	#endif
	TmrInsertTimer(GetTmrHd(), ReloadCnt, PollingCOCalibrationStartCheck, NULL);
	SetSysSleepFlag(eActiveFlagCO);
	
    
}
/*****************************************************************************
*???????????? : PollingCOCalibrationLedFlick
*???????????? : ????????????co data
*???????????? : Handle:	???????????????
          : eCmd 	??????
*???????????? : ???
*???????????? : 
*****************************************************************************/
static void PollingCOCalibrationLedFlick(TmrHdl Handle, eTmrCmd eCmd)
{
	uint32_t LedType = (uint32_t)TmrGetCtx(Handle);
	switch(eCmd)
	{
		case eTimeOut:
			{			
				COGetData();
				switch (LedType)
				{
					case LED_TYPE_RED:
						LedPollingHandle(LED_TYPE_RED, 50);
						break;
					case LED_TYPE_GREEN:
						LedPollingHandle(LED_TYPE_GREEN, 50);
						break;
					case LED_TYPE_YELLOW:
						LedPollingHandle(LED_TYPE_YELLOW, 50);
						break;
				}
			}
			break;
			
		default:
			break;
	}
}
#ifdef SUPPORT_WIFI_NET_FUNCTION

uint8_t CheckPPMLEVEL(uint16_t PPM)
{
	static uint8_t Result=0;
	if(PPM<COPPMLEVEL0)//APP Clear 0
	{
		Result=LEVEL;
	}
	else if(PPM>COPPMLEVEL0&&PPM<=COPPMLEVEL1)// <70ppm
	{
		Result=LEVEL0;
	}
	else if(PPM>COPPMLEVEL1&&PPM<=COPPMLEVEL2)// <70ppm
	{
		Result=LEVEL1;
	}
	else if(PPM>COPPMLEVEL2&&PPM<=COPPMLEVEL3)// <70ppm
	{
		Result=LEVEL2;
	}
	else if(PPM>COPPMLEVEL3&&PPM<=COPPMLEVEL4)// <150ppm
	{
		Result=LEVEL3;
	}
	else// >COPPMLEVEL4
	{
		Result=LEVEL4;
	}
	return Result;
}

static void COUpdateWIFIPPM(COTag *pCO)//WIFI Update PPM For IDLE 
{
	static uint8_t OLDPPMLEVEL=0;

	if(OLDPPMLEVEL!=CheckPPMLEVEL(pCO->Ppm))
	{
		OLDPPMLEVEL=CheckPPMLEVEL(pCO->Ppm);
		if(OLDPPMLEVEL==LEVEL)
		{
			pCO->Ppm=0;
		}
		#ifdef WiFiUpdataNew
		SetWiFiUpdata(DPID_CO_VALUE,pCO->Ppm,0xff,0,0xff,0);
		WiFiOpenCheck();
		#else
		Get_DpId_Data(DPID_CO_VALUE,pCO->Ppm);
		#endif
	}
}
static void COupdateWIFIPPM(COTag *pCO)
{
	static uint16_t LastPpm = 0;			// CO PPM 
	if(CoObject.Ppm!=LastPpm)
	{
		LastPpm = CoObject.Ppm;
		#ifdef WiFiUpdataNew
		SetWiFiUpdata(DPID_CO_VALUE,pCO->Ppm,0xff,0,0xff,0);
		WifiAO.WifiCONNFlag=TRUE;
		#else
		Get_DpId_Data(DPID_CO_VALUE,CoObject.Ppm);
		#endif
	}
}

#endif

#ifdef SUPPORT_NET_FUNCTION

static void COupdateNetPPM(COTag *pCO)
{
	static uint16_t LastPpm = 0;			// CO PPM 
	if( (SystemGetNetMode()==ePairedHub)&&(CoObject.Ppm!=LastPpm) )
	{
		#ifdef	Debug_Test
			printf("Net_ppm:%d\n",CoObject.Ppm);
		#endif
		LastPpm = CoObject.Ppm;			
		UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgUpdateCoPpm, (CoObject.Ppm&0xFF00)>>8, CoObject.Ppm&0xFF);		
	}
}

#endif

#ifdef SUPPORT_UL2034_UL217
/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/
static uint16_t COPpmULCheck(COTag *pCO,uint16_t Ppm)
{
	uint16_t ret = eCONO;//0 :normal 1:smoke 2:err
	static uint8_t CoCancelAlarmNum = 0;
	if(Ppm<=COPPMLEVEL1)// <30ppm
	{
		CoCancelAlarmNum++;
		if(CoCancelAlarmNum>3)
		{
			pCO->COAlarmThreshold0 = 0;
			pCO->COAlarmThreshold1 = 0;
			pCO->COAlarmThreshold2 = 0;
			pCO->COAlarmThreshold3 = 0;
			pCO->COAlarmThreshold4 = 0;
			if(pCO->State != eCONO)
    		{
				pCO->State = eCONO;
				COEvt.sig = eSystemEventCOAlarmCancel;
				SystemPostFifo(&COEvt);
			}
		}

	}
	else  //>=30 ppm
	{

		CoCancelAlarmNum = 0;
		if(Ppm>COPPMLEVEL1&&Ppm<=COPPMLEVEL2)// <70ppm
		{
			pCO->COAlarmThreshold0++;

		}
		else if(Ppm>COPPMLEVEL2&&Ppm<=COPPMLEVEL3)// <150ppm
		{
			pCO->COAlarmThreshold0++;
			pCO->COAlarmThreshold1++;

		}else if(Ppm>COPPMLEVEL3&&Ppm<=COPPMLEVEL4)// <400ppm
		{
			pCO->COAlarmThreshold0++;
			pCO->COAlarmThreshold1++;
			pCO->COAlarmThreshold2++;

		}else// >5000ppm
		{
			pCO->COAlarmThreshold0++;
			pCO->COAlarmThreshold1++;
			pCO->COAlarmThreshold2++;
			pCO->COAlarmThreshold3++;
		}
		
	   if((pCO->COAlarmThreshold0 > (CO_ALARM_70PPM_TIME*60/(SLEEP_TIME*CO_CHECK_COUNT))) ||
       (pCO->COAlarmThreshold1 > (CO_ALARM_150PPM_TIME*60/(SLEEP_TIME*CO_CHECK_COUNT))) ||
       (pCO->COAlarmThreshold2 > (CO_ALARM_400PPM_TIME*60/(SLEEP_TIME*CO_CHECK_COUNT))) ||
       (pCO->COAlarmThreshold3 > (CO_ALARM_400PPM_TIME*60/(SLEEP_TIME*CO_CHECK_COUNT))))
	    {  
	    	if(pCO->State != eCOed)
    		{
				COEvt.sig = eSystemEventCOAlarm;
				SystemPostFifo(&COEvt);	
				pCO->State = eCOed;
    		}
			ret = eCOed;
			
		#ifdef Debug_Test			
		printf("COAlarmThreshold0 = %d\n",pCO->COAlarmThreshold0);
		printf("COAlarmThreshold1 = %d\n",pCO->COAlarmThreshold1);
		printf("COAlarmThreshold2 = %d\n",pCO->COAlarmThreshold2);
		printf("COAlarmThreshold3 = %d\n",pCO->COAlarmThreshold3);			
		#endif

	    }
	   
	}
	 
	return ret;
}
#else
/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

static uint16_t COPpmBSICheck(COTag *pCO,uint16_t Ppm)
{
	uint16_t ret = eCONO;//0 :normal 1:smoke 2:err
	static uint8_t CoCancelAlarmNum = 0;
	if(Ppm<=COPPMLEVEL0)// <30ppm
	{
		CoCancelAlarmNum++;
		if(CoCancelAlarmNum>3)
		{
			pCO->COAlarmThreshold0 = 0;
			pCO->COAlarmThreshold1 = 0;
			pCO->COAlarmThreshold2 = 0;
			pCO->COAlarmThreshold3 = 0;
			if(pCO->State != eCONO)
    		{
				pCO->State = eCONO;
				COEvt.sig = eSystemEventCOAlarmCancel;
				SystemPostFifo(&COEvt);
			}
			
		}

	}
	else  //>=30 ppm
	{
		CoCancelAlarmNum = 0;
		if(Ppm>COPPMLEVEL0&&Ppm<=COPPMLEVEL1)// <50ppm
		{
			pCO->COAlarmThreshold0++;

		}
		else if(Ppm>COPPMLEVEL1&&Ppm<=COPPMLEVEL2)// <100ppm
		{
			pCO->COAlarmThreshold0++;
			pCO->COAlarmThreshold1++;

		}else if(Ppm>COPPMLEVEL2&&Ppm<=COPPMLEVEL3)// <300ppm
		{
			pCO->COAlarmThreshold0++;
			pCO->COAlarmThreshold1++;
			pCO->COAlarmThreshold2++;

		}
		else if(Ppm>COPPMLEVEL3&&Ppm<=COPPMLEVEL4)// <5000ppm
		{
			pCO->COAlarmThreshold0++;
			pCO->COAlarmThreshold1++;
			pCO->COAlarmThreshold2++;
			pCO->COAlarmThreshold3++;

		}else// >5000ppm
		{
			pCO->COAlarmThreshold0++;
			pCO->COAlarmThreshold1++;
			pCO->COAlarmThreshold2++;
			pCO->COAlarmThreshold3++;
		}
		
#ifdef SUPPORT_CO_DEBUG
			printf("COAlarmThreshold0 = %d\n",pCO->COAlarmThreshold0);
			printf("COAlarmThreshold1 = %d\n",pCO->COAlarmThreshold1);
			printf("COAlarmThreshold2 = %d\n",pCO->COAlarmThreshold2);
			printf("COAlarmThreshold3 = %d\n",pCO->COAlarmThreshold3);
	
#endif
	   if((pCO->COAlarmThreshold0 > (CO_ALARM_30PPM_TIME*60/(SLEEP_TIME*CO_CHECK_COUNT))) ||
       (pCO->COAlarmThreshold1 > (CO_ALARM_50PPM_TIME*60/(SLEEP_TIME*CO_CHECK_COUNT))) ||
       (pCO->COAlarmThreshold2 > (CO_ALARM_100PPM_TIME*60/(SLEEP_TIME*CO_CHECK_COUNT))) ||
       (pCO->COAlarmThreshold3 > (CO_ALARM_300PPM_TIME*60/(SLEEP_TIME*CO_CHECK_COUNT))))
	    {       
			if(pCO->State != eCOed)
    		{
				COEvt.sig = eSystemEventCOAlarm;
				SystemPostFifo(&COEvt);	
				pCO->State = eCOed;
    		}
			ret = eCOed;
			pCO->COAlarmThreshold0 = 0;
			pCO->COAlarmThreshold1 = 0;
			pCO->COAlarmThreshold2 = 0;
			pCO->COAlarmThreshold3 = 0;
	    }
	   
	}
	 
	return ret;
}
#endif
/*****************************************************************************
*???????????? : COGetPPM
*???????????? : ??????ppm??????
*???????????? : pCO	 	???CO??????     
*???????????? : ppm???
*???????????? : 
*****************************************************************************/
static uint16_t COGetPPM(COTag *pCO)
{
	uint16_t ret = 0;
	uint32_t temp_offset=(uint32_t)(pCO->COCaliOffset);
	uint32_t temp_slope=(uint32_t)(pCO->COCaliGradient);
	uint16_t offset=0;
	uint16_t slope=0;
	uint16_t COData = COAdcGetSample(pCO,CO_NORMAL_CHECK);
	uint8_t Temp=SensorGetTemperature();
	
	
	//co????????????
	#ifdef SUPPORT_CoTempCompensation
	offset=(uint16_t)(temp_offset*COTempOffsetRate[Temp]/1000);//b
	slope=(uint16_t)(temp_slope*COTempSlopeRate[Temp]/1000);//k
	#else
	offset = (uint16_t)temp_offset;//b
	slope  = (uint16_t)temp_slope;//k
	#endif
	
	if(COData>=offset)
	{
		ret = (COData - offset)*10/slope;//x=(y-b)/k
		
	}
	else
	{
		ret = 0;
	}
	//ret = 168;//for test
	
	#ifdef Debug_Test
  		printf("\nco sample:%.3f(%d)\n",MVoltageConvertToVoltage(COData),COData);
		printf("co:%d ppm\n",ret);//*
		printf("T:%dC\n",Temp-20);
	#endif
	return ret;
}

/*****************************************************************************
*???????????? : COEnvironmentCompensation
*???????????? : CO ????????????
*???????????? : Ppm	 	???ppm???     
*???????????? : ppm???
*???????????? : 
*****************************************************************************/

#ifdef  BSI_CERTIFY_FAIL_PROCESS	
static uint16_t COEnvironmentCompensation(uint16_t Ppm)
{
	uint16_t ret = Ppm;
	uint8_t Temp=SensorGetTemperature();
	if(Temp < 18)			// < -2   ??C, + 20%
	{
		Ppm +=  (Ppm * 20) / 100;
		#ifdef SUPPORT_CO_DEBUG
		if(IsUartOpen())
		{
	    	printf("\nlow compensate co Ppm:%d\n",Ppm);
		}
		#endif
		
	}
	else if(Temp > 55)		// > 35 ??C, - 5%
	{
		Ppm -=  (Ppm * 5) / 100;
		#ifdef SUPPORT_CO_DEBUG
		if(IsUartOpen())
		{
	    	printf("\nhigh compensate co Ppm:%d\n",Ppm);
		}
		#endif				
	}else
	{
		//
	}
	ret = Ppm;
	return ret;
}
#endif

#ifdef SUPPORT_CO_TEMP_TEST
/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void CO_Temperature_Test(uint16_t co_ppm)
{
		uint8_t i=0;

		static uint8_t CoTempFlag=0;
	
#if defined SUPPORT_UL2034_UL217
		static uint32_t Ppm70_Count=0;
		static uint32_t Ppm150_Count=0;
		static uint32_t Ppm400_Count=0;
#else
		static uint32_t Ppm30_Count=0;
		static uint32_t Ppm50_Count=0;
		static uint32_t Ppm100_Count=0;
		static uint32_t Ppm300_Count=0;
#endif
		static uint32_t TempPpm=0;
		static uint16_t AvrPpm=0;

		uint8_t temperature_data = SensorGetTemperature();

    #if defined SUPPORT_UL2034_UL217
    if((CoTempFlag==0)&&(co_ppm>300) && (co_ppm<500))        // 400ppm
    {
        Ppm70_Count=0;
        Ppm150_Count=0;
        Ppm400_Count++;
        if((Ppm400_Count>=60)&&(Ppm400_Count<90))        //10min
        {
            TempPpm += co_ppm;
        }
        else if(Ppm400_Count==90)        //15min
        {
            CoTempFlag=1;
            AvrPpm = TempPpm/30;
            if(temperature_data<30)     // < 10C
            {
                i=6;
            }
            else
            {
                i=0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x10+i, AvrPpm>>8);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x11+i, AvrPpm&0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm=0;
        }
    }
    else if((CoTempFlag==0)&&(co_ppm>110) && (co_ppm<190))   // 150ppm
    {
        Ppm70_Count=0;
        Ppm400_Count=0;
        Ppm150_Count++;
        if((Ppm150_Count>=60)&&(Ppm150_Count<90))        //10min
        {
            TempPpm += co_ppm;
        }
        else if(Ppm150_Count==90)        //15min
        {
            AvrPpm = TempPpm/30;
            if(temperature_data<30)     // < 10C
            {
                i=6;
            }
            else
            {
                i=0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x12+i, AvrPpm>>8);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x13+i, AvrPpm&0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm=0;
        }
    }
    else if((CoTempFlag==0)&&(co_ppm>50) && (co_ppm<90))     // 70ppm
    {
        Ppm400_Count=0;
        Ppm150_Count=0;
        Ppm70_Count++;
        if((Ppm70_Count>=60)&&(Ppm70_Count<90))        //10min
        {
            TempPpm += co_ppm;
        }
        else if(Ppm70_Count==90)        //15min
        {
            AvrPpm = TempPpm/30;
            if(temperature_data<30)     // < 10C
            {
                i=6;
            }
            else
            {
                i=0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x14+i, AvrPpm>>8);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x15+i, AvrPpm&0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm=0;
        }
    }
    else
    {
        Ppm70_Count=0;
        Ppm150_Count=0;
        Ppm400_Count=0;
        TempPpm=0;
    }
    #elif defined SUPPORT_EN50291_EN14604
    if((CoTempFlag==0)&&(co_ppm>200) && (co_ppm<400))        // 300ppm
    {
        Ppm30_Count=0;
        Ppm50_Count=0;
        Ppm100_Count=0;
        Ppm300_Count++;
        if((Ppm300_Count>=60)&&(Ppm300_Count<90))        //10min
        {
            TempPpm += co_ppm;
        }
        else if(Ppm300_Count==90)        //15min
        {
            CoTempFlag=1;
            AvrPpm = TempPpm/30;
            if(temperature_data<30)     // < 10C
            {
                i=8;
            }
            else
            {
                i=0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x10+i, AvrPpm>>8);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x11+i, AvrPpm&0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm=0;
        }
    }
    else if((CoTempFlag==0)&&(co_ppm>70) && (co_ppm<130))   // 100ppm
    {
        Ppm30_Count=0;
        Ppm50_Count=0;
        Ppm300_Count=0;
        Ppm100_Count++;
        if((Ppm100_Count>=60)&&(Ppm100_Count<90))        //10min
        {
            TempPpm += co_ppm;
        }
        else if(Ppm100_Count==90)        //15min
        {
            AvrPpm = TempPpm/30;
            if(temperature_data<30)     // < 10C
            {
                i=8;
            }
            else
            {
                i=0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x12+i, AvrPpm>>8);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x13+i, AvrPpm&0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm=0;
        }
    }
    else if((CoTempFlag==0)&&(co_ppm>40) && (co_ppm<70))     // 50ppm
    {
        Ppm300_Count=0;
        Ppm100_Count=0;
        Ppm30_Count=0;
        Ppm50_Count++;
        if((Ppm50_Count>=60)&&(Ppm50_Count<90))        //10min
        {
            TempPpm += co_ppm;
        }
        else if(Ppm50_Count==90)        //15min
        {
            AvrPpm = TempPpm/30;
            if(temperature_data<30)     // < 10C
            {
                i=8;
            }
            else
            {
                i=0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x14+i, AvrPpm>>8);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x15+i, AvrPpm&0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm=0;
        }
    }
    else if((CoTempFlag==0)&&(co_ppm>20) && (co_ppm<40))     // 30ppm
    {
        Ppm300_Count=0;
        Ppm100_Count=0;
        Ppm50_Count=0;
        Ppm30_Count++;
        if((Ppm30_Count>=60)&&(Ppm30_Count<90))        //10min
        {
            TempPpm += co_ppm;
        }
        else if(Ppm30_Count==90)        //15min
        {
            AvrPpm = TempPpm/30;
            if(temperature_data<30)     // < 10C
            {
                i=8;
            }
            else
            {
                i=0;
            }
            FLASH_Unlock(FLASH_MemType_Data);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x16+i, AvrPpm>>8);
            FLASH_ProgramByte(FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+0x17+i, AvrPpm&0xff);
            FLASH_Lock(FLASH_MemType_Data);
        }
        else
        {
            TempPpm=0;
        }
    }
    else
    {
        Ppm30_Count=0;
        Ppm50_Count=0;
        Ppm100_Count=0;
        Ppm300_Count=0;
        TempPpm=0;
    }
    #endif
}
#endif




/*****************************************************************************
*???????????? : COAlarmCheck
*???????????? : CO ????????????
*???????????? : pCO	 	???CO??????     
*???????????? : ppm???
*???????????? : 
*****************************************************************************/

uint16_t COAlarmCheck(COTag *pCO)
{
  uint16_t ret = eCONO;//0 :normal 1:smoke 2:err
  //uint16_t PpmDiff = 0;
  uint16_t Ppm = 0;
  #ifdef SUPPORT_LCD_FUNCTION
  static uint8_t state =0;
  #endif
  assert_param(NULL==pCO);
  
  Ppm = COGetPPM(pCO);
  
  #ifdef  BSI_CERTIFY_FAIL_PROCESS
  Ppm = COEnvironmentCompensation(Ppm);
  #endif
    
  if(Ppm<COPPMLEVEL0)// display from COPPMLEVEL0
  {
    pCO->Ppm = 0;
    #ifdef SUPPORT_LCD_FUNCTION
      if(state !=0)
      {
        state =0;
        LCD_DisplayNum(0);
      }else
      {
        ;
      }
    #endif
  }
  else
  {
  
    #ifdef SUPPORT_LCD_FUNCTION
      if(state >= 2)
      {
       // printf("lcd update co:%d ppm\n",Ppm);
        state =2;
        LCD_Control(eLcdON);
        BatteryLcdDisplay();
        LCD_DsiplayCoFlg(eLcdON);
        LCD_DisplayNum(Ppm);
		#ifdef SUPPORT_WIFI_NET_FUNCTION
			COUpdateWIFIPPM(&CoObject);
		#endif
      }else
      {
        state ++;  
      //  printf("state = %d \n",state);
        }
    #endif
  
    }
  pCO->Ppm = Ppm;
  #ifdef SUPPORT_CO_TEMP_TEST
   CO_Temperature_Test(Ppm);
  #else
    #ifdef SUPPORT_UL2034_UL217
    ret = COPpmULCheck(pCO,Ppm);    
    #else
    ret = COPpmBSICheck(pCO,Ppm);
    #endif
  #endif
  return ret;
}

/*****************************************************************************
*???????????? : IsCOErr
*???????????? : CO????????????
*???????????? : 
*???????????? : Ret???1?????? 0??????
*???????????? : 
*****************************************************************************/
FlagStatus IsCOErr(void)
{
	FlagStatus Ret = RESET;
	if((CoObject.State==eCOOPEN)||(CoObject.State==eCOSHORT))
	{
		Ret = SET;
	}
	return Ret;
}




/*****************************************************************************
*???????????? : COSelfCheckHandle
*???????????? : CO????????????
*???????????? : pCO co????????????
*???????????? : ret://0 :normal 1:CO 2:err
*???????????? : 
*****************************************************************************/
static uint16_t COSelfCheckHandle(COTag *pCO)
{
	uint16_t ret = eCONO;//0 :normal 1:CO 2:err
	uint16_t COData = 0;
	uint16_t COShortData = 0;

	int16_t Temp = 0;
	uint8_t TempC = SensorGetTemperature();
	assert_param(NULL==pCO);
	COData=COAdcGetSample(pCO,CO_NORMAL_CHECK);
	COShortData=COAdcGetSample(pCO,CO_SHORT_FAULT_CHECK);	
	Temp = COShortData-COData;
	
	#ifdef Debug_Test
	printflog("COData = %d	COShortData = %d\n",COData,COShortData);
	#endif
	if((COShortData>SENSOR_OPEN_FAULT_SAMPLE)&&(TempC<74))
    {        
        DelayMs(20);
        COData=COAdcGetSample(pCO,CO_NORMAL_CHECK);	
		
        if(COData<SENSOR_OPEN_SAMPLE)
        {
            pCO->COFaultOpenCount++;//??????
            pCO->COFaultShortCount=0;
			pCO->State = eCOErr;
        }
        else
        {
            ;
        }
    }
    else if(((COShortData>SENSOR_SHORT_SAMPLE_MIN)&&(COShortData<SENSOR_SHORT_SAMPLE_MAX))&&((Temp<100)&&(Temp>-100))
        &&(TempC<74))
    {
        pCO->COFaultShortCount++;//??????
        pCO->COFaultOpenCount=0;
		pCO->State = eCOErr;
    }
    else
    {
        if(IsCOErr())
		{
			COEvt.sig = eSystemEventCOAlarmCancel;
			SystemPostFifo(&COEvt);
		}
		pCO->COFaultOpenCount=0;
        pCO->COFaultShortCount=0;
        pCO->State = eCONO;
		
    }

    if(pCO->COFaultOpenCount>=2)
    {
		if(!IsCOErr())
		{
			COEvt.sig = eSystemEventCOErr;
			SystemPostFifo(&COEvt);
		}
		pCO->COFaultOpenCount = 2;
        pCO->State = eCOOPEN;
        
    }
    else if(pCO->COFaultShortCount>=2)
    {
    	if(!IsCOErr())
		{
			COEvt.sig = eSystemEventCOErr;
			SystemPostFifo(&COEvt);
		}
		pCO->COFaultShortCount = 2;
        pCO->State = eCOSHORT;
		
    }
	
	ret = pCO->State;	
	return ret;
}

/*****************************************************************************
*???????????? : COSelfCheck
*???????????? : CO??????
*???????????? : 
*???????????? : 
*???????????? : 
*****************************************************************************/
void COSelfCheck(void)
{	
	uint32_t SlpTimeCnt = SystemGetIdleNUm();	

	if( (CoObject.COFaultShortCount)||(CoObject.COFaultOpenCount))//error
	{
		COSelfCheckHandle(&CoObject);
		
	}
	else//OK
	{
		if(((SlpTimeCnt%CO_CHECK_COUNT)==(CO_CHECK_COUNT-1))) //30s time  check  2
		{	 
			COAlarmCheck(&CoObject);
			#ifdef SUPPORT_NET_FUNCTION
				COupdateNetPPM(&CoObject);			
			#endif
			
		}
		if((SlpTimeCnt%CO_SELFCHECK_COUNT)==(CO_SELFCHECK_COUNT-1)) //60s same time check   5
	    { 
	       COSelfCheckHandle(&CoObject);  
		  
		}
	}
	
}



/*****************************************************************************
*???????????? : PollingCOBeep
*???????????? : CO????????????
*???????????? : Handle:???????????????
          : eCmd 	??????
*???????????? : ???
*???????????? : 
*****************************************************************************/
static void PollingCOBeep(TmrHdl Handle, eTmrCmd eCmd)
{
	uint32_t Time = (uint32_t)TmrGetCtx(Handle);
	static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut: 
			if(i++<CO_ALARM_COUNT)
			{				
				LedPollingHandle(LED_TYPE_RED,Time);
				BeepPollingHandle(100);
				//printf("PollingCOBeep = %d\n", i);
			}else
			{
				i = 0;
				ResetSysSleepFlag(eActiveFlagCOBeep);
				TmrDeleteTimer(Handle);	
				COEvt.sig = eSystemEventCOBeepOK;
				SystemPostFifo(&COEvt);
				//printf("eSystemEventCOBeepOK\n");
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
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/
static void PollingCOBeepDelay(TmrHdl Handle, eTmrCmd eCmd)
{
	switch(eCmd)
	{
		case eTimeOut:	
			ResetSysSleepFlag(eActiveFlagCOBeepDelay);
			TmrDeleteTimer(Handle);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOBeep, ((void*)100));
			SetSysSleepFlag(eActiveFlagCOBeep);
			break;
			
		case eDelTimer:
			break;	
			
		default:
			break;
	}
}

/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

static void PollingCOMute(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;
	static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut:	
			i++;
			if(i>=2)
			{
				i = 0;
				TmrDeleteTimer(Handle);
				COEvt.sig = eSystemEventCOMuteTimeout;
				SystemPostFifo(&COEvt);			
				
			}else
			{
				COAlarmCheck(&CoObject);
				if(CoObject.Ppm>COPPMLEVEL1)
				{	
					COEvt.sig = eSystemEventCOAlarm;
					SystemPostFifo(&COEvt);

				}
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
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

static void PollingCOMuteLed(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;
	static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut:
			if(i++<CO_ALARM_COUNT)
			{
				LedPollingHandle(LED_TYPE_RED,100);
				
			}else
			{
				i = 0;
				ResetSysSleepFlag(eActiveFlagCOBeep);
				TmrDeleteTimer(Handle);	
				COEvt.sig = eSystemEventCOMuteLedOK;
				SystemPostFifo(&COEvt);				
			}
			break;
		case eDelTimer:
			i = 0;
			break;	
		default:
			break;
	}
}
static void CO_PollingCloseLcdBackLightDelay(TmrHdl Handle, eTmrCmd eCmd)
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

/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

static void PollingCOMuteLedDelay(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;

	switch(eCmd)
	{
		case eTimeOut:	
			ResetSysSleepFlag(eActiveFlagCOBeepDelay);
			TmrDeleteTimer(Handle);				
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOMuteLed, NULL);
			SetSysSleepFlag(eActiveFlagCOBeep);
			break;
		case eDelTimer:
			break;	
		default:
			break;
	}
}

void COLcdDisplay(void)
{
#ifdef SUPPORT_LCD_FUNCTION
	if(CoObject.Ppm >=30)
	{
		LCD_DisplayNum(CoObject.Ppm);
		#ifdef Debug_Test
		printflog("----CoObject.Ppm = %d\n",CoObject.Ppm);
		#endif
	}else
	{
		LCD_DisplayNum(0);
	}
#endif
}
static void PollingCOTestDisplay(TmrHdl Handle, eTmrCmd eCmd)
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
			if(i<12)
			{
				if(j==9)
				{
		
				}
				else if(j%3==0)
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
				#endif
					j++;
				}
				++i;
			}
			else
			{
				i = 0;
				j= 0;
				TmrDeleteTimer(Handle);
			}
			break;
			
		case eDelTimer:
			j = 0;
			i = 0;
			break;	
		default:
			break;
	}
}

static void PollingCOTestDisplayDelay(TmrHdl Handle, eTmrCmd eCmd)
{
	switch(eCmd)
	{
		case eTimeOut:
			TmrDeleteTimer(Handle);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_TEST_LCD_TIME), PollingCOTestDisplay, NULL);
			break;
			
		case eDelTimer:
			break;

		default:
			break;
	}
}


/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void COKeyTestDelay(uint8_t Flag)// flag =1  start delay other is beepdely
{
    uint16_t COState = eCONO;
    switch(Flag)
    {

		case 1:
			COState = COSelfCheckHandle(&CoObject);
			if(IsCOErr()||(eCOErr==COState))
			{
				COEvt.sig = eSystemEventCOErr;
				SystemPostFifo(&COEvt);
				CoObject.State = eCOOPEN;
			}
			else
			{	
				TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), PollingCOTestDisplayDelay, NULL);
				TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_KEY_TEST_DELAY_TIME), PollingCOBeepDelay, NULL);
				SetSysSleepFlag(eActiveFlagCOBeepDelay);
			}
			break;
	
		default :
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEPDELAY_TIME), PollingCOBeepDelay, NULL);
			SetSysSleepFlag(eActiveFlagCOBeepDelay);
			break;	
		
	}
}

//dzs
static void CO_PollingMuteAlarm(TmrHdl Handle, eTmrCmd eCmd)
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
				ResetSysSleepFlag(eActiveFlagCoMuteAlarmLed);
				COEvt.sig = eSystemEventCoMuteExitTimeout;
				SystemPostFifo(&COEvt);
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
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/
static void SystemCOMute(SystemApiTag *me,QEvent *e)
{
	static int8_t DelayUpdata=0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:	
			#ifdef Debug_Test
			printf("%s in,%d\n",__FUNCTION__,__LINE__);
			#endif
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			DelayUpdata=2;

			#else
			Get_DpId_Data(DPID_MUFFLING,MuteStateOn);
			#endif
			#endif
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_BackLightControl(eLcdBackLightON);
			#endif
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_MUTE_TIME), PollingCOMute, NULL);
		    SetSysSleepFlag(eActiveFlagCO);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOMuteLed, NULL);
			SetSysSleepFlag(eActiveFlagCOBeep);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CLOSE_LCD_BACKLIGHTTIME), CO_PollingCloseLcdBackLightDelay, NULL);//////
			SetSysSleepFlag(eActiveFlagCloseLcdBackLight);//////
			break;

		case Q_EXIT_SIG:
			#ifdef Debug_Test
			printf("%s out,%d\n",__FUNCTION__,__LINE__);
			#endif
			
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOMute));
			ResetSysSleepFlag(eActiveFlagCO);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOMuteLed));
			ResetSysSleepFlag(eActiveFlagCOBeep);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOMuteLedDelay));
			ResetSysSleepFlag(eActiveFlagCOBeepDelay);
			CoObject.State = eCONO;
		break;
			
	    case eSystemEventCOMuteTimeout:
			#ifdef SUPPORT_NET_FUNCTION		
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0, 0);				
			#endif

			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_MUFFLING,MuteStateOff,DPID_CO_STATE,CONormal,DPID_CO_VALUE,CoObject.Ppm);
			WiFiOpenCheck();
			#else
			Get_DpId_Data(DPID_MUFFLING,MuteStateOff);//Cancel Mute & Cancel Alarm
			#endif
			#endif
			
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_BackLightControl(eLcdBackLightON);
			COLcdDisplay();
			#endif
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOMuteLed));
			ResetSysSleepFlag(eActiveFlagCOBeep);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOMuteLedDelay));
			ResetSysSleepFlag(eActiveFlagCOBeepDelay);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), CO_PollingMuteAlarm, NULL);
			SetSysSleepFlag(eActiveFlagCoMuteAlarmLed);
			break;

		case eSystemEventCoMuteExitTimeout: //3?????????
			Q_TRAN(SysIdleModeHandleEvent);
			break;			
		
		case eSystemEventCOAlarm:
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_MUFFLING,MuteStateOff,DPID_CO_STATE,COAlarm,DPID_CO_VALUE,CoObject.Ppm);
			WiFiOpenCheck();
			#else
			Get_DpId_Data(DPID_MUFFLING,MuteStateOff);//Cancel Mute & Cancel Alarm
			#endif
			#endif
			Q_TRAN(SystemCOAlarm);	
			break;

			
		case eSystemEventCOErr:
			Q_TRAN(SysIdleModeHandleEvent);
			break;

			
		case eSystemEventCOMuteLedOK:
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEPDELAY_TIME), PollingCOMuteLedDelay, NULL);
			SetSysSleepFlag(eActiveFlagCOBeepDelay);
			
			CoObject.Ppm =  COGetPPM(&CoObject);
			if(CoObject.Ppm<COPPMLEVEL0)
			{
				CoObject.Ppm = 0;
			}
			#ifdef SUPPORT_LCD_FUNCTION
	        LCD_DisplayNum(CoObject.Ppm);	
			#endif
			#ifdef SUPPORT_NET_FUNCTION
			COupdateNetPPM(&CoObject);
			#endif
			
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			if(DelayUpdata>0)
			{
				DelayUpdata--;
			}else
			{
				COUpdateWIFIPPM(&CoObject);
			}
			#endif
			break;
			
		#ifdef SUPPORT_NET_FUNCTION
		//app ??????????????????
		case eNETEventCancelMute:
			if(SystemGetNetMode()==ePairedHub)
			{
				Q_TRAN(SysIdleModeHandleEvent); 
			}			 	
		break;
		
		case eNETEventCoAlarm:	
				Q_TRAN(NetCOAlarm);
		break;
		
		case eNETEventSmokeAlarm:		
				Q_TRAN(NetSmokeAlarm); 	
		break;
		#endif
			
		default:
			
			break;
	}

}

//dzs
static void CO_PollingCancelAlarm(TmrHdl Handle, eTmrCmd eCmd)
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
				COEvt.sig = eSystemEventCOAlarmCancelTimeout;
				SystemPostFifo(&COEvt);
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
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

static void PollingCOErrIndex(TmrHdl Handle, eTmrCmd eCmd)
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

				}else
				{
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

FlagStatus IsCOPPM(void)
{
	FlagStatus Ret = RESET;
	if(CoObject.Ppm<COPPMLEVEL0)
	{
		Ret = SET;
	}
	return Ret;
}


/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

static void PollingCOErr(TmrHdl Handle, eTmrCmd eCmd)
{
	switch(eCmd)
	{
		case eTimeOut:
			{
				TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOErrIndex, NULL);
				SetSysSleepFlag(eActiveFlagDevErr);
			}
			break;
			
		default:
			break;
	}
}


/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void SystemCOError(SystemApiTag  *me,QEvent *e)
{	
	switch (e->sig)
	{
		case Q_eSystemEventWakeup:	
			#ifdef SUPPORT_SMOKE_FUNCTION
			CheckSmokerAlarm(FALSE,TRUE);
			#endif
			COSelfCheck();
		    SysTimeUpdate();	
			SystemPollingIdle();
			#ifdef Support_keytesterror_debug
				printf("eSystemEventWakeup, %s, %d\r\n",__FUNCTION__,__LINE__);
			#endif
			break;
			
		case Q_ENTRY_SIG:
			#ifdef SUPPORT_LCD_FUNCTION
				LCD_DsiplayStr(eStr_Err, 0, 0);//"Err"
			#endif
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOErrIndex, NULL);
			SetSysSleepFlag(eActiveFlagDevErr);
			TmrInsertTimer(GetTmrIdleHd(), TMR_IDLE_PERIOD(CO_ERR_TIME), PollingCOErr, NULL);
			
			#ifdef SUPPORT_NET_FUNCTION
				if(SystemGetNetMode()==ePairedHub)//hub
				{
					UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgFault, DevCoSenorFault, 0);
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
			#ifdef Debug_Test
			printf("Event:co Error\n");
			#endif
			break;
			
		case Q_EXIT_SIG:
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrIdleHd(),PollingCOErr));
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingCOErrIndex)); 
			ResetSysSleepFlag(eActiveFlagDevErr);

			#ifdef SUPPORT_WIFI_NET_FUNCTION
			SetWiFiUpdata(DPID_FAULT,faultCancel,0xff,0,0xff,0);
			WiFiOpenCheck();
			#endif
			
			#ifdef SUPPORT_NET_FUNCTION
				if(SystemGetNetMode()==ePairedHub)//hub
				{
					UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgFault, DevCancelFault, 0);
				}
			#endif	
			#ifdef SUPPORT_LCD_FUNCTION
				LCDStandyDisplay();
			#endif
			#ifdef Debug_Test
			printf("%s out,%d\n",__FUNCTION__,__LINE__);
			#endif
			break;
		
		case eSystemEventCOAlarmCancel:
			Q_TRAN(SysIdleModeHandleEvent);
			break;
		
			#ifdef SUPPORT_SMOKE_FUNCTION
		case Q_eSmokeEventAlarm:

			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_SMOKE_SENSOR_STATE,SmokeIs,0xff,0,0xff,0);
			WiFiOpenCheck();
			#else
			Get_DpId_Data(DPID_SMOKE_SENSOR_STATE,SmokeIs);
			#endif
			#endif
			Q_TRAN(SysSmokeAlarmModeEvent);
			break;
			#endif
#if 1
		case Q_eKeyEventKey1Clik:
			#ifdef Support_keytesterror_debug
				printf("eSystemEventKey, %s, %d\r\n",__FUNCTION__,__LINE__);
			#endif
			{
				Q_TRAN(SystemKeyTestHandle);	
			}
			break;
#endif			
		default:
			break;
	}

}

/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void COKeyTest(void)
{
	TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOBeep, ((void*)100));
	SetSysSleepFlag(eActiveFlagCOBeep);
	#ifdef SUPPORT_CO_DEBUG
	if(IsUartOpen())
	{		
		printf("%s ,%d\n",__FUNCTION__,__LINE__);
	}
	#endif	
}




/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

#ifdef SUPPORT_NET_FUNCTION


static void Net_PollingCOBeep(TmrHdl Handle, eTmrCmd eCmd)
{
	uint32_t Time = (uint32_t)TmrGetCtx(Handle);
	static uint8_t i = 0;
	
	switch(eCmd)
	{
		case eTimeOut:
			if(i++<CO_ALARM_COUNT)
			{
				LedPollingHandle(LED_TYPE_RED,Time);
				BeepPollingHandle(100);
			}else
			{
				i = 0;
				ResetSysSleepFlag(eActiveFlagCOBeep);
				TmrDeleteTimer(Handle);	
				COEvt.sig = eSystemEventCOBeepOK;
				SystemPostFifo(&COEvt);		

				SetSysSleepFlag(eActiveFlagNetKeyTestGREENLED);
        		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(110), PollingLedGreen, ((void*)200));//
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
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/
static void Net_PollingCOBeepDelay(TmrHdl Handle, eTmrCmd eCmd)
{
	Handle = Handle;
	switch(eCmd)
	{
		case eTimeOut:	
			ResetSysSleepFlag(eActiveFlagCOBeepDelay);
			TmrDeleteTimer(Handle); 			
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), Net_PollingCOBeep, ((void*)100));
			SetSysSleepFlag(eActiveFlagCOBeep);
			break;
			
		case eDelTimer:
			break;	
			
		default:
			break;
	}
}




/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void NetCOAlarm(SystemApiTag  *me,QEvent *e)
{	
	switch (e->sig)
	{
	
		case Q_ENTRY_SIG:
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), Net_PollingCOBeep, ((void*)100));
			SetSysSleepFlag(eActiveFlagCOBeep);
			#ifdef DEBUG_NET
				printf("%s in,%d\n",__FUNCTION__,__LINE__);
			#endif
			break;
			
		case Q_EXIT_SIG:

			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), Net_PollingCOBeep));
			ResetSysSleepFlag(eActiveFlagCOBeep);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), Net_PollingCOBeepDelay));
			ResetSysSleepFlag(eActiveFlagCOBeepDelay);
			#ifdef DEBUG_NET
				printf("%s out,%d\n",__FUNCTION__,__LINE__);			
			#endif
			break;

		case Q_eKeyEventKey1Clik:
			#ifdef SUPPORT_NET_FUNCTION
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0, 0);
				Q_TRAN(SysIdleModeHandleEvent);
			#endif
			
		break;
		   
		case eNETEventMute:
			#ifdef SUPPORT_RF_NET_FUNCTION
			Q_TRAN(SysIdleModeHandleEvent);
			#else
			COEvt.sig = eSystemEventCOAlarmCancel;
			SystemPostFifo(&COEvt);
			Q_TRAN(SysIdleModeHandleEvent);
			#endif
			break;	
			
		case eSystemEventCOBeepOK:
			#ifdef SUPPORT_SMOKE_FUNCTION
			CheckSmokerAlarm(FALSE,FALSE);
			#endif
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEPDELAY_TIME), Net_PollingCOBeepDelay, NULL);
			SetSysSleepFlag(eActiveFlagCOBeepDelay);
			COAlarmCheck(&CoObject);
			break;
			
		case eNETEventCancleAlarm:
			Q_TRAN(SysIdleModeHandleEvent);
			break;
			
			
		case Q_eSmokeEventAlarm:
			CoObject.State = eCONO;
			#ifdef SUPPORT_SMOKE_FUNCTION
			Q_TRAN(SysSmokeAlarmModeEvent);
			#endif
			break;
			
		case eSystemEventCOAlarm:
			Q_TRAN(SystemCOAlarm);	
			break;
		case eNETEventSmokeAlarm://goto Netsmoke
			Q_TRAN(NetSmokeAlarm);
		break;
		default:
			break;
	}

}
#endif


/*****************************************************************************
*???????????? : SystemCOAlarm
*???????????? : CO??????????????????
*???????????? : me:??????
          : e ??????????????????
*???????????? : ???
*???????????? : 
*****************************************************************************/
void SystemCOAlarm(SystemApiTag  *me,QEvent *e)
{	
	static uint16_t Num = 0;
	static uint8_t  Counter=0;
	
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			#ifdef Debug_Test
			printf("in %s ,%d\n",__FUNCTION__,__LINE__);
			#endif
			
			#ifdef SUPPORT_NET_FUNCTION
			if(IsNetEnable())
			{
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCoAlarm, (CoObject.Ppm&0xFF00)>>8,CoObject.Ppm&0xFF);
			}
			#endif
			Counter=2;
			
	        #ifdef SUPPORT_LCD_FUNCTION
			LCD_BackLightControl(eLcdBackLightON);
			#endif		
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEP_TIME), PollingCOBeep, ((void*)100));
			SetSysSleepFlag(eActiveFlagCOBeep);	
			#ifdef SUPPORT_EMC_TEST
			TmrInsertTimer(GetTmrIdleHd(), TMR_IDLE_PERIOD(CO_CALIBRATION_ERR_TIME), PollingCOErr, NULL);
			#endif
			CoObject.State = eCOed;
			break;
			
		case Q_EXIT_SIG:
			//??????co????????????
			CoObject.State = eCONO;
			#ifdef Debug_Test
			printflog("out %d\n",__LINE__);
			#endif
			#ifdef SUPPORT_LCD_FUNCTION
				LCD_BackLightControl(eLcdBackLightOFF);
			#endif
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeep));
			ResetSysSleepFlag(eActiveFlagCOBeep);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeepDelay));
			ResetSysSleepFlag(eActiveFlagCOBeepDelay);
			break;

		case Q_eKeyEventKey1Clik:
		#ifdef SUPPORT_NET_FUNCTION
		case eNETEventMute:
		#endif

		#ifdef SUPPORT_WIFI_NET_FUNCTION
		#if defined SUPPORT_EN50291_EN14604
		if(COGetPPM(&CoObject)<COPPMLEVEL3)
		#endif
		{
			SetWiFiUpdata(DPID_MUFFLING,MuteStateOn,DPID_CO_VALUE, CoObject.Ppm,0xff,0);
//			WiFiOpenCheck();
			WifiAO.WifiCONNFlag=TRUE;
			Q_TRAN(SystemCOMute);
		}
		#endif
		
		break;
		
		#ifdef SUPPORT_WIFI_NET_FUNCTION
		case eWIFINETEventMute:
		#if defined SUPPORT_EN50291_EN14604
		if(COGetPPM(&CoObject)<COPPMLEVEL3)
		#endif
		{
			WifiAO.WifiCMDTimeout=4;
			Q_TRAN(SystemCOMute);
		}
		#if defined SUPPORT_EN50291_EN14604
		else
		{
			SetWiFiUpdata(DPID_MUFFLING,MuteStateOff,DPID_CO_VALUE, CoObject.Ppm,0xff,0);
			WifiAO.WifiCONNFlag=TRUE;
		}
		#endif
		#endif
		break;
		
		case eSystemEventCOBeepOK:
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(CO_BEEPDELAY_TIME), PollingCOBeepDelay, NULL);
			SetSysSleepFlag(eActiveFlagCOBeepDelay);
			#ifdef SUPPORT_SMOKE_FUNCTION
			CheckSmokerAlarm(FALSE,TRUE);
			#endif	
			if(CoObject.State==eCONO)//cancel alarm
			{
				COEvt.sig = eSystemEventCOAlarmCancel;
				SystemPostFifo(&COEvt);		
			}else
			{
				COAlarmCheck(&CoObject);	
			}
			++Num;
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			if( Num%2==0)
			{
				if(Counter>0)
				{
					Counter--;
				}
				else
				{
					COupdateWIFIPPM(&CoObject);
				}
				
			}
			#endif
			#ifdef SUPPORT_NET_FUNCTION
			if( Num%2==0)
			{										
				COupdateNetPPM(&CoObject);			   				
			}
			#endif
			
			break;
			
		case eSystemEventCOAlarmCancel:
			#ifdef SUPPORT_LCD_FUNCTION
			LCD_DisplayNum(CoObject.Ppm);
			#endif
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeep));
			ResetSysSleepFlag(eActiveFlagCOBeep);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), PollingCOBeepDelay));
			ResetSysSleepFlag(eActiveFlagCOBeepDelay);			
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), CO_PollingCancelAlarm, NULL);
			SetSysSleepFlag(eActiveFlagCancelAlarmLed);
			
			#ifdef SUPPORT_NET_FUNCTION
			if(IsNetEnable())
			{
				//????????????
				UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgCancelAlarm, 0, 0);	
			}
			#endif
			
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_CO_STATE,CONormal,DPID_CO_VALUE,CoObject.Ppm,0xff,0);
			WifiAO.WifiCONNFlag=TRUE;
			#else
			Get_DpId_Data(DPID_CO_STATE,CONormal);
			#endif
			#endif
			break;

		case eSystemEventCOAlarmCancelTimeout://?????????
			COEvt.sig = eSystemEventCOAlarmCancel;
			SystemPostFifo(&COEvt);
			Q_TRAN(SysIdleModeHandleEvent);
			break;

			
		case eSystemEventCOErr:
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			SetWiFiUpdata(DPID_FAULT,sensor_fault,0xff,0,0xff,0);
			WifiAO.WifiCONNFlag=TRUE;
			#endif
			Q_TRAN(SystemCOError);
			break;
	
		case Q_eSmokeEventAlarm:
			#ifdef SUPPORT_WIFI_NET_FUNCTION
			SetWiFiUpdata(DPID_SMOKE_SENSOR_STATE,SmokeIs,0xff,0,0xff,0);
			WifiAO.WifiCONNFlag=TRUE;
			#endif
			#ifdef SUPPORT_SMOKE_FUNCTION
			Q_TRAN(SysSmokeAlarmModeEvent);
				#ifdef Debug_Test
				printf("co alrm test smoke alarm\n");
				printflog("CoObject.State = %d\n",CoObject.State);
				#endif
			
			#endif
			break;
			
		default:
			break;
	}

}

/*****************************************************************************
*???????????? : SysCoCailModeHandleEvent
*???????????? : CO??????????????????
*???????????? : me:??????
          : e ??????????????????
*???????????? : ???
*???????????? : 
*****************************************************************************/
void SysCoCailModeHandleEvent(SystemApiTag  *me, QEvent *e)
{
	static uint16_t LCD_PPM=0;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			printflog("%s (in)\n", __FUNCTION__);
			#ifdef SUPPORT_LCD_FUNCTION 
			LCD_Control(eLcdON);
			#endif	
			CoCaliObject.COCaliState = FlashGetCOState();
			CoCaliObject.COCaliGradient = FlashGetCOGradient();
			CoCaliObject.COCaliOffset = FlashGetCOOffset();

			if(CoCaliObject.COCaliState == CO_CALI_STATE)
			{
				/* ????????????co */
				CoObject.COCaliGradient = CoCaliObject.COCaliGradient;//??????flash?????????k???
				CoObject.COCaliOffset = CoCaliObject.COCaliOffset;	//??????flash?????????b???

				COEvt.sig = eSystemEvenCalibrationSmoke;
				SystemPostFifo(&COEvt);
			}
			else
			{	

				/* ????????????co */
				COEvt.sig = eSystemEvenCalibrationCO;
				SystemPostFifo(&COEvt);
			
			}
			break;
		case Q_EXIT_SIG:
			printflog("%s (out)\n", __FUNCTION__);
			break;

		case Q_eSystemEventWakeup:
			SysTimeUpdate();
	    	SystemPollingIdle();
			#ifdef Debug_Test
				printflog("co cail wake\n");
			#endif	
			break;
		case eSystemEvenCalibrationCO:
			if (GetBatteryData()>BATTERY_FACTORY_CHECK)
			{						
				COCalibrationCheckStart(&CoCaliObject);									
			}
			else
			{
				COEvt.sig = eSystemEvenCalibrationCoBatteryErr;
				SystemPostFifo(&COEvt);			
			}
			#ifdef SUPPORT_CO_DEBUG
			printf("%s ,%d\n",__FUNCTION__,__LINE__);
			#endif
			break;	

		case Q_eSmokeEventCailSuccess:
			Q_TRAN(&SysIdleModeHandleEvent);
			break;

		case eSystemEvenCalibrationCOOK:
			if(CoCaliObject.COCaliState>=2)			//step3
			{	
				//printf("step 3\n");
				FlashSetCOState(CO_CALI_STATE);
			 	CoObject.COCaliGradient = FlashGetCOGradient();
				CoObject.COCaliOffset = FlashGetCOOffset();
				#ifdef Debug_Test
					printf("COCaliOffset:%d,%d\n",CoCaliObject.COCaliOffset,CoObject.COCaliOffset);
					printf("COCaliGradient:%d,%d\n",CoCaliObject.COCaliGradient,CoObject.COCaliGradient);
				#endif
				LCD_PPM=COGetPPM(&CoObject);
//				COAlarmCheck(&CoObject);		
				#ifdef SUPPORT_LCD_FUNCTION
					LCD_Clear();
					LCD_DisplayNum(LCD_PPM);
				#endif
			}
			else
			{		
				CoCaliObject.COCaliState++;	
				FlashSetCOState(CoCaliObject.COCaliState);		
				if(CoCaliObject.COCaliState==1)			//step1
				{
					//printf("step 1\n");
					FlashSetCOOffset(CoCaliObject.Sample);
					CoCaliObject.COCaliOffset = CoCaliObject.Sample;
					CoObject.COCaliOffset = CoCaliObject.COCaliOffset;
					#ifdef Debug_Test
						printf("COCaliOffset:%d,%d\n",CoCaliObject.COCaliOffset,CoObject.COCaliOffset);		
					#endif
					#ifdef SUPPORT_LCD_FUNCTION
						LCD_Clear();
						LCD_DisplayNum(CoObject.COCaliOffset);
					#endif
				}
				else									//step2
				{	//printf("step 2\n");
					CoCaliObject.Sample = CoCaliObject.COCaliOffset>CoCaliObject.Sample?CoCaliObject.COCaliOffset - CoCaliObject.Sample:CoCaliObject.Sample - CoCaliObject.COCaliOffset;
					CoCaliObject.COCaliGradient = CoCaliObject.Sample/15;
					CoObject.COCaliGradient = CoCaliObject.COCaliGradient;
					FlashSetCOGradient(CoCaliObject.COCaliGradient);//  gardient/10ppm
					
					#ifdef Debug_Test
					printf("COCaliGradient:%d,%d\n",CoCaliObject.COCaliGradient,CoObject.COCaliGradient);
					#endif
					#ifdef SUPPORT_LCD_FUNCTION
						LCD_Clear();
						LCD_DisplayNum(CoObject.COCaliGradient);
					#endif
				}
			}
			
			TmrInsertTimer(GetTmrHd(), TMR_IDLE_PERIOD(2000), PollingCOCalibrationLedFlick, (void*)LED_TYPE_GREEN);	
			
			break;
		case eSystemEvenCalibrationCoBatteryErr:
			#ifdef SUPPORT_LCD_FUNCTION 
			LCD_DsiplayStr(eStr_E12, 0, 2);
			#endif
			#ifdef SUPPORT_CO_DEBUG
			printf("%s CalibrationTempErr,%d\n",__FUNCTION__,__LINE__);
			#endif

			TmrInsertTimer(GetTmrHd(), TMR_IDLE_PERIOD(2000), PollingCOCalibrationLedFlick, (void*)LED_TYPE_RED);	
			
			break;
		case eSystemEvenCalibrationCOErr:
			#ifdef SUPPORT_LCD_FUNCTION
            LCD_Clear(); 
			switch(CoCaliObject.COCaliState)
			{
				
				case 0:
					//LCD_DisplayNum(COGetData());
					LCD_DsiplayStr(eStr_E08, 0, 2);					
					break;
				
				case 1:				
					//LCD_DisplayNum(COGetData());
					LCD_DsiplayStr(eStr_E09, 0, 2);
					break;
				
				case 2:
					LCD_DsiplayStr(eStr_E10, 0, 2);
					break;
			}	
			#endif
			#ifdef Debug_Test
			printf("%s CalibrationCOErr,%d\n",__FUNCTION__,__LINE__);
			#endif
			
			TmrInsertTimer(GetTmrHd(), TMR_IDLE_PERIOD(2000), PollingCOCalibrationLedFlick, (void*)LED_TYPE_RED);	
			break;
			
		case eSystemEvenCalibrationTempErr:
			#ifdef SUPPORT_LCD_FUNCTION				
			LCD_DsiplayStr(eStr_E06, 0, 2);
			#endif
			#ifdef Debug_Test
			printf("%s CalibrationTempErr,%d\n",__FUNCTION__,__LINE__);
			#endif
			TmrInsertTimer(GetTmrHd(), TMR_IDLE_PERIOD(2000), PollingCOCalibrationLedFlick, (void*)LED_TYPE_RED);	
			break;

		case eSystemEvenCalibrationSmoke:
			Q_TRAN(SystemStartHandle);
			//Q_TRAN(SysIdleModeHandleEvent);//??????????????????????????????
			
			break;
		
		default:
			#ifdef SUPPORT_CO_DEBUG
				printf("%s ,%d,%d\n",__FUNCTION__,__LINE__,e->sig);
			#endif
			break;
	}

}


bool IsUartOpen()
{
  return FALSE;
}


