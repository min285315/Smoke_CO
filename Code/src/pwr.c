#include"pwr.h"
#include"adc.h"
#include"lcd.h"
#include"led.h"
#include"beep.h"
#include"RF.h"
#include "protocol.h"


#include"WiFiUpdata.h"

static uint8_t BatteryState = eBatteryLevelThree;
static BatteryAOTag BatteryAO = {0};

void SetBatteryLowState(uint8_t State)
{
	BatteryAO.State = State;
}

uint8_t GetBatteryLowState(void)
{
	return BatteryAO.State;
}

void BatteryInit(void)
{
	BatteryAO.State = 1;
	CheckBatteryLevel();
	printflog("BatterySample = %d\n",BatteryAO.BatterySample);
}


uint8_t GetBatteryState(void)
{

	return BatteryState;
}



FlagStatus IsLowPwrErr(void)
{
	FlagStatus Ret = RESET;
	if( (BatteryAO.State == 0)&&(BatteryState==eBatteryLevelNull) )
	{
		Ret = SET;
	}
	return Ret;
}



/**********************************************************************/
//Name:			GetBatteryData()
//Description:															
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
uint16_t GetBatteryData(void)
{
	uint16_t BatteryMCUSample = 0;

	BAT_DETECT_ON();
	BatteryMCUSample = AdcGetChannelValue(ADC_SamplingTime_384Cycles, ADC_Channel_20);
	BAT_DETECT_OFF();
	BatteryMCUSample=AdcConvertTomVoltage(BatteryMCUSample*2);

	return BatteryMCUSample;
}

/**********************************************************************/
//Name:			CheckBatteryLevel()
//Description:																
//Parameters:                        										
//Return:   																
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
uint8_t CheckBatteryLevel(void)
{

	uint16_t BatterySample = 0;
	
	BatterySample = GetBatteryData();
	BatteryAO.BatterySample = BatterySample;
	if(BatterySample>=BatteryState_FULL)
	{
		BatteryState=eBatteryLevelThree;
	}
	else if( (BatterySample<BatteryState_FULL)
		   &&(BatterySample>=BatteryState_TWO_THIRED))
	{
		BatteryState=eBatteryLevelTwo;
	}
    else if( (BatterySample<BatteryState_TWO_THIRED)
  		  &&(BatterySample>=BatteryState_ONE_THIRED))
    {
		BatteryState=eBatteryLevelOne;
    }
	else if( BatterySample<BatteryState_ONE_THIRED )
    {
		BatteryState=eBatteryLevelNull;
    }

	return BatteryState;
}

static void BatteryCheckHandle(void)
{
	#ifdef SUPPORT_WIFI_NET_FUNCTION
	static uint8_t BatteryStateOld=4;
	#endif
	
	uint8_t BatteryStateCurrent = CheckBatteryLevel();//
	#ifdef SUPPORT_WIFI_NET_FUNCTION
	if(BatteryStateOld!=BatteryStateCurrent)
	{
		BatteryStateOld=BatteryStateCurrent;
		if(BatteryStateCurrent==eBatteryLevelThree)
		{
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_BATTERY_STATE,BatteryHig,0xff,0,0xff,0);
			#else
			Get_DpId_Data(DPID_BATTERY_STATE,BatteryHig);
			#endif
		}else if((BatteryStateCurrent==eBatteryLevelOne)||(BatteryStateCurrent==eBatteryLevelTwo))
		{
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_BATTERY_STATE,BatteryMid,0xff,0,0xff,0);
			#else
			Get_DpId_Data(DPID_BATTERY_STATE,BatteryMid);
			#endif
		}else
		{
			#ifdef WiFiUpdataNew
			SetWiFiUpdata(DPID_BATTERY_STATE,BatteryLow,0xff,0,0xff,0);
			#else
			Get_DpId_Data(DPID_BATTERY_STATE,0);
			#endif
		}
		WiFiOpenCheck();
	}
	#endif
	if( (BatteryStateCurrent < eBatteryLevelOne)&&(BatteryAO.State) )// state change
	{	
		if(BatteryAO.LowBatCheckCnt>=2)//3 time
		{
			BatteryAO.LowBatCheckCnt = 0;
			BatteryState = BatteryStateCurrent;
			if(BatteryState==eBatteryLevelNull)
			{
				BatteryAO.State = 0;
				#ifdef SUPPORT_NET_FUNCTION
				if(SystemGetNetMode()==ePairedHub)//hub
				{
					UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgLowBat, BatteryState, 0);
				}
				#endif
				
			}
		}
		else
		{
			BatteryAO.LowBatCheckCnt++;
		}
				
	}
	else
	{
		BatteryAO.LowBatCheckCnt = 0;// for emc 	
	}
	
	#ifdef Debug_Test
		printflog("BatterySample = %d\n\n",BatteryAO.BatterySample);
	#endif	
	#ifdef SUPPORT_LCD_FUNCTION
	BatteryLcdDisplay();//

	#endif
}
void BatterCheck(void)
{	
	static uint32_t LifeTimeDaysLast  = 0;
	uint32_t LifeTimeDays = SystemGetLifeDays();
	uint32_t SlpTimeCnt = SystemGetIdleNUm();

	
	if(LifeTimeDaysLast>=2)
	{
		if((LifeTimeDaysLast  != LifeTimeDays)||((BatteryState!=eBatteryLevelNull)&&(BatteryAO.LowBatCheckCnt>0))||(BatteryState==eBatteryLevelNull))
		{
			LifeTimeDaysLast = LifeTimeDays;		
			BatteryCheckHandle();
		}
	}else//10 s system wakeup noe time
	{		   
        if(((SlpTimeCnt%BATTERY_CHECK_COUNT)==(BATTERY_CHECK_COUNT-4)) || ((BatteryState!=eBatteryLevelNull)&&(BatteryAO.LowBatCheckCnt>0))||(BatteryState==eBatteryLevelNull)) //4：for check no same time     
        {
           BatteryCheckHandle();   
    	}
		if(SlpTimeCnt % SYSTEM_ONE_DAY_COUNT ==(SYSTEM_ONE_DAY_COUNT-1))
    	{
    		LifeTimeDaysLast++;
    	}
	}

}



#ifdef SUPPORT_LCD_FUNCTION

void BatteryLcdDisplay(void)
{
	
	switch (BatteryState)
	{
		case eBatteryLevelNull:
			//LCD_DsiplayStr(eStr_Lb,0,0);
			LCD_DisplayBatteryData(eLcdBatteryFlgLevel_Null);
			break;
			
		case eBatteryLevelOne:
			LCD_DisplayBatteryData(eLcdBatteryFlgLevel_1);
			break;
		
		case eBatteryLevelTwo:
			LCD_DisplayBatteryData(eLcdBatteryFlgLevel_2);
			break;
		
		case eBatteryLevelThree:
			LCD_DisplayBatteryData(eLcdBatteryFlgLevel_3);
			break;

		default:
			break;
	}
}
#endif



