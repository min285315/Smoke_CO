#include "Module.h"
#include "wifi.h"
#include "SceneInstance.h"
#include "main.h"
#include "system.h"
//#include "mcu_api.h"
#ifdef SUPPORT_WIFI_NET_FUNCTION
static QEvent WIFIEVENT;
WifiObjectTag WifiAO;
#ifdef WiFiUpdataNew
DATAWIFI UpdataWifi[UpdataMax];
#endif


void WifiVarInit(void)
{  
	WifiAO.WifiUpdateState = 0xff;
	WifiAO.WifiUpdateValue = 0xff;
	WifiAO.WifiResetFlag = FALSE;
	WifiAO.WifiCONNFlag = FALSE;
	WifiAO.PirState=FALSE;
	WifiAO.WifiUpdateFlag = FALSE;// need update  flag
	WifiAO.WifiCheckUpdateFlag = TRUE;//check update  flag
	WifiAO.eUpgradeFlag = (eResetMode)FlashGeteUpgradeFlag();
//	WifiAO.eUpgradeFlag=(eResetMode)FLASH_ReadByte(FLASH_DATA_EEPROM_ADDR_CALI_FLAG+22);

	WifiAO.Wifiheartbeat = FALSE;
	WifiAO.UpgradeTimeoutCnt = 0;
	WifiAO.WifiCMDTimeout = 0;
	WifiAO.WifiFactory = FlashGetWifiFactory();
//	WifiAO.WifiFactory=FLASH_ReadByte(FLASH_DATA_EEPROM_ADDR_CALI_FLAG+23);
	WifiAO.WifiPiredFlag = FALSE;
	WifiAO.WifiReconnectNum = 0;
	WifiAO.FirstOpenFlag = 1;// connect timeout
	ReSetWiFiUpdata();

	GPIO_Init(WIFI_POWER_PORT, WIFI_POWER_PIN, GPIO_Mode_Out_PP_Low_Slow);
	WIFI_POWER_OFF();
	
}

static void WIFIconnectedTimeout(void)
{
	WifiAO.WifiCheckUpdateFlag = FALSE;
	if(WifiAO.WifiReconnectNum>=WIFIRECONNECTEDTIMES)
	{
		WifiAO.WifiPiredFlag = FALSE;
		WifiAO.WifiReconnectNum = 0;
	}else
	{
		WifiAO.WifiReconnectNum++;
	}
	#ifdef SUPPORT_NET_FUNCTION_DEBUG
	printf("\nWifiReconnectNum:%d\n", (int)WifiAO.WifiReconnectNum);
	#endif
}


void CalibrationOKIndex(void)
{
//	printf("1\n");
	FlashSetWifiFactory(FACTRORY_CHECKWIFI_FLAG);
//	printf("2\n");
	while(1)
	{
//		printf("3\n");
		IWDG_ReloadCounter();
//		printf("4\n");
		RED_LED_OFF();	
//		printf("5\n");
		G_LED_TURN_ON();//index
//		printf("6\n");
		DelayMs(500);
//		printf("7\n");
		G_LED_TURN_OFF();
//		printf("8\n");
		DelayMs(500);
//		printf("9\n");
	}
}
void CalibrationErrIndex(void)
{
	while(1)
	{	
		IWDG_ReloadCounter();
		G_LED_TURN_OFF();	
		RED_LED_ON();//index
		BEEP_TURN_ON();
		DelayMs(500);
		RED_LED_OFF();	
		BEEP_TURN_OFF()
		DelayMs(500);	
	}	
}
/**********************************************************************/
//Description:		NetConnetPolling   														  
//Parameters:         Time :led on time               												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
void NetConnetPolling(TmrHdl Handle, eTmrCmd eCmd)//配网
{
	#define LedFlickerCounter (480)// 120s
	uint16_t state =(uint16_t)TmrGetCtx(Handle);
	static uint16_t RfTestLedFlickerNum = 0;
	static uint8_t WifiLaststate  = WIFI_SATE_UNKNOW;
	
	uint8_t Wifistate = mcu_get_wifi_work_state();
	
	printflog("state:%d\n", Wifistate);
	switch(eCmd)
	{
		case eInsertTimer:
			break;
		case eDelTimer:
			break;
		case eTimeOut:
			if (RfTestLedFlickerNum>=LedFlickerCounter)
			{
				RfTestLedFlickerNum = 0;
				WifiAO.FirstOpenFlag = 1;// connect timeout
				TmrDeleteTimer(Handle);
				ResetSysSleepFlag(eActiveFlagNetConnetTimeout);
				WifiDeinit();
				if(WifiAO.PirState)
				{
					WifiAO.PirState=0;
					WIFIEVENT.sig=eSystemEventExitPir;
					SystemPostFifo(&WIFIEVENT);
				}
				
				WIFIconnectedTimeout();
			}else
			{
				++RfTestLedFlickerNum;		
				if(WifiAO.eUpgradeFlag == eAPMode)
				{
					if(RfTestLedFlickerNum%4==0)//1S
					{
						if((GetCOState()==eCOed)||(GetSmokeState()==eSmokeState_Alarm))
						{
							;
						}
						else
						{
							LedPollingHandle(LED_TYPE_GREEN,state);
						}
						
					}
				}else//250ms
				{
					if((GetCOState()==eCOed)||(GetSmokeState()==eSmokeState_Alarm))
					{
						;
					}
					else
					{
						LedPollingHandle(LED_TYPE_GREEN,state);
					}
				}
			}
			
			switch(Wifistate)
			{
				case SMART_CONFIG_STATE:
				{
					if(WifiLaststate == WIFI_CONNECTED)
					{
						WifiAO.FirstOpenFlag = 1;//delete and connect
					}
						
					WifiLaststate = SMART_CONFIG_STATE;
					if(WifiAO.eUpgradeFlag == eAPMode)
					{	
					//	mcu_set_wifi_mode(AP_CONFIG);
					//	FlashSeteUpgradeFlag(eAPMode);
					//	RfTestLedFlickerNum = 0;
						WIFIEVENT.sig =eSystemEventWifiPairStart;
						SystemPostFifo(&WIFIEVENT);
					}
					WifiAO.WifiResetFlag = FALSE;
				}
			//	printflog("state-0\n");
				break;
				
				case AP_STATE:
				{
					if(WifiLaststate == WIFI_CONNECTED)
					{
						WifiAO.FirstOpenFlag = 1;//delete and connect
					}
					WifiLaststate = AP_STATE;
					if(WifiAO.eUpgradeFlag == eSTAMode)
					{
					//	RfTestLedFlickerNum = 0;
						WIFIEVENT.sig =eSystemEventWifiPairStart;
						SystemPostFifo(&WIFIEVENT);
					}
					WifiAO.WifiResetFlag = FALSE;	
				}
			//	printflog("%d\n",1);
				break;
				
				case WIFI_NOT_CONNECTED:
				{
					if(WifiLaststate == WIFI_CONNECTED)
					{
						WifiAO.FirstOpenFlag = 1;//delete and connect
					}
						
					if(WifiAO.WifiResetFlag == TRUE)
					{
						WifiAO.WifiResetFlag = FALSE;
						RfTestLedFlickerNum = 0;
					}
					WifiLaststate = WIFI_NOT_CONNECTED;//	
				}
			//	printflog("%d\n",2);
				break;
						
				case WIFI_CONNECTED:
				{
					WifiLaststate = WIFI_CONNECTED;
				}
			//	printflog("%d\n",3);
				break;
						
				case WIFI_CONN_CLOUD:
				{
					WifiLaststate = WIFI_CONN_CLOUD;
					RfTestLedFlickerNum = 0;
					WifiAO.WifiPiredFlag = TRUE;
					WifiAO.WifiReconnectNum = 0;
					TmrDeleteTimer(Handle);
					ResetSysSleepFlag(eActiveFlagNetConnetTimeout);
					if((GetSmokeState()==eSmokeState_Null)&&(GetCOState()==eCONO))
					{
						//LedPollingHandle(LED_TYPE_GREEN,1000);
					}
					if(WifiAO.FirstOpenFlag==1)
					{
						#ifdef WiFiUpdataNew
						APPInit();
						#else
						Smoke_all_data_update();
						#endif
					}else
					{
						WifiAO.WifiCONNFlag = TRUE;
						WIFIEVENT.sig=eSystemEventWifiPairSuccess;
						SystemPostFifo(&WIFIEVENT);
					}
					if(WifiAO.PirState)
					{
						WifiAO.PirState=0;
						WIFIEVENT.sig=eSystemEventExitPir;
						SystemPostFifo(&WIFIEVENT);
					}
					#ifdef	SUPPORT_UPGRADE_FIRMWARE
					if(WifiAO.WifiCheckUpdateFlag)
					{
						firmware_upgrade_check();
						PollingFirmwareUpgade(1000);
					}
					#endif
				}
				break;
				default:
					break;
				}
		break;
	}
	
}
void PollingWifiTimeout(TmrHdl Handle, eTmrCmd eCmd)//上传成功删除polling
{
	#ifndef WiFiUpdataNew
	uint16_t Curstate = (uint16_t)TmrGetCtx(Handle);
	uint8_t Wifistate = Curstate&0xff;
	#endif
	unsigned char WifiupdataState=mcu_get_wifi_work_state();
	switch(eCmd)
	{
		case eTimeOut:
		if((GetSmokeState()==eSmokeState_Alarm)||(GetCOState()==eCOed))
		{
		
		}
		else
		{
			if(WifiAO.WifiCMDTimeout<5)
			{
				if(WifiAO.WifiCMDTimeout%2==1)
				{
					#ifdef WiFiUpdataNew
					Mcu_dp_Update_All();
					#else
					WIFIStateHandle(Wifistate);
					#endif
				}
				WifiAO.WifiCMDTimeout++;
			}
			else
			{
				WifiAO.WifiCMDTimeout = 0;
				if(!WifiAO.WifiCheckUpdateFlag)
				{
					WifiDeinit();
				}
				ResetSysSleepFlag(eActiveFlagWifiTimeout);
				ReSetWiFiUpdata();
				TmrDeleteTimer(Handle);
//				printflog("Delete WifiTimeout\n");
//				TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),NetConnetPolling));
			}
		}
		break;
	}
}
void WifiInit(void)
{
	if(!WifiAO.WifiUpdateFlag)
	{
		WIFI_POWER_ON();//open wifi
		wifi_protocol_init();
		#if 0
		if((GetSmokeState()==eSmokeState_Alarm||GetCOState()==eCOed)&&(!WifiAO.Wifiheartbeat))
		{
			G_LED_TURN_ON();
		}
		#endif
		SetSysSleepFlag(eActiveFlagNetConnetTimeout);
		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(RFLEDFLASHNUM),NetConnetPolling,(void*)RFLEDFLASHON);
		#if 1 //def	SUPPORT_NET_FUNCTION_DEBUG
		printf("Wifiopen\n");
		#endif
	}
}

void WifiDeinit(void)
{
	if (!WifiAO.WifiUpdateFlag)
	{
		WIFI_POWER_OFF();    //close wifi	
		wifi_protocol_init();	
		WifiAO.Wifiheartbeat = FALSE;
		#if 1 //def	SUPPORT_NET_FUNCTION_DEBUG
		printf("Wifioff\n");
		#endif
	}
	G_LED_TURN_OFF();
}

void SmokeSetAllUpdateFlag(void)
{
  WifiAO.FirstOpenFlag = 1;
  WifiAO.WifiReconnectNum = 0;
}

void WiFi_EN(uint8_t config)
{
	WifiAO.WifiResetFlag = TRUE;
	WifiAO.FirstOpenFlag = 1;
	WifiDeinit();
	WifiInit(); 							
	WifiAO.eUpgradeFlag = (eResetMode)config;	
	DelayMs(500);
	G_LED_TURN_OFF();
	DelayMs(500);
	SystemPolling();
	if(WifiAO.eUpgradeFlag == eSTAMode)
	{
		mcu_set_wifi_mode(SMART_CONFIG_STATE);
		FlashSeteUpgradeFlag(eSTAMode);
	}else
	{
		mcu_set_wifi_mode(AP_STATE);
		FlashSeteUpgradeFlag(eAPMode);
	}
	BeepPollingHandle(100);
}

void WIFIAP(void)
{
	if(!WifiAO.WifiUpdateFlag)
	{
		if((mcu_get_wifi_work_state()==SMART_CONFIG_STATE)||(mcu_get_wifi_work_state()==WIFI_NOT_CONNECTED))
		{
			WifiAO.WifiResetFlag = FALSE;
			mcu_set_wifi_mode(AP_CONFIG);	
			FlashSeteUpgradeFlag(eAPMode);
			WifiAO.eUpgradeFlag = eAPMode;
			BeepPollingHandle(100); 
		}else
		{
			WiFi_EN(eAPMode);
		}
	}
}

void WIFIEZ(void)
{
	if(!WifiAO.WifiUpdateFlag)
	{
		if((mcu_get_wifi_work_state()==AP_CONFIG)||(mcu_get_wifi_work_state()==WIFI_NOT_CONNECTED))
		{
			WifiAO.WifiResetFlag = FALSE;
			mcu_set_wifi_mode(SMART_CONFIG_STATE);
			FlashSeteUpgradeFlag(eSTAMode);
			WifiAO.eUpgradeFlag = eSTAMode;
			WifiAO.FirstOpenFlag = 1;
			BeepPollingHandle(100);
		}
		else
		{
			WiFi_EN(eSTAMode);
		}
	}
}

#ifdef SUPPORT_UPGRADE_FIRMWARE
void PollingFirmwareUpgade(uint16_t Time)
{
	WIFI_POWER_ON();
	SetSysSleepFlag(eActiveFlagWifiUpgradeTimeout);
	TmrInsertTimer(GetTmrHd(),TMR_PERIOD(Time),PollingWifiUpgradeTimeout,NULL);
}

void PollingWifiUpgradeTimeout(TmrHdl Handle, eTmrCmd eCmd)

{
	unsigned char DeleteFlag = 0;
	unsigned char WifiUpgradeState= mcu_get_wifi_upgrade_state();
	#ifdef SUPPORT_NET_FUNCTION_DEBUG
	printf("\n WifiUpgradeState:%d\n",(int)WifiUpgradeState);
	#endif
	switch(eCmd)
	{
		case eTimeOut:
			switch (WifiUpgradeState)
			{
				case WIFI_FIRM_START_CHECKING:					//Request again
				break;
				case WIFI_FIRM_IS_UPGRADING:					//Set Update Flag
				WifiAO.WifiUpdateFlag = TRUE;
				G_LED_TURN_ON();
				if(WifiAO.UpgradeTimeoutCnt%5 == 0)						//5s check one time
				{
					firmware_upgrade_check();
				}
				break;
		
				case WIFI_FIRM_UPGRADE_FAIL:					//ignore
			     DeleteFlag = 1;
				break;

				case WIFI_FIRM_IS_LAST:							//close
			     DeleteFlag = 1;
				break;
				case WIFI_FIRM_UPGRADE_SUCCESS:					//soft reset
				if(mcu_get_wifi_work_state()==WIFI_CONN_CLOUD)
				{
					DeleteFlag = 1;
				}	
				#ifdef SUPPORT_NET_FUNCTION_DEBUG
				printf("\nWifiCheckUpdateFlag:%d,state:%d\n", (int)WifiAO.WifiCheckUpdateFlag,(int)mcu_get_wifi_work_state());
				#endif
				break;
				default:
				break;
			}
		WifiAO.UpgradeTimeoutCnt++ ;

		if(WifiAO.UpgradeTimeoutCnt> SUPPORT_UPGRADE_FIRMWARE_TIMEOUT)//300S								//60s
		{
			DeleteFlag = 1;
		}

		if(DeleteFlag == 1)
		{
			WifiAO.UpgradeTimeoutCnt = 0;
			TmrDeleteTimer(Handle);
			ResetSysSleepFlag(eActiveFlagWifiUpgradeTimeout);
			WifiAO.WifiUpdateFlag = FALSE;
			WifiAO.WifiCheckUpdateFlag = FALSE;
			WifiDeinit();
		}
		break;
	}
}
#endif

void WiFiCheckOneDay(uint32_t time)//half One Day
{
	uint32_t Sleep_Time_Cnt=time;
	static uint16_t LifeDays=0;

	#define  WIFIRECONNECTEDNUM    (4)
	#define  WIFIRECONNECTEDUPDATE (30)
	#define  WIFISELFCHECKUPDATE   (14)

	if(Sleep_Time_Cnt%WiFiHalfONEDAY==WiFiHalfONEDAY-1)
	{
		LifeDays++;
		WifiAO.Wifiheartbeat = TRUE;
		if(LifeDays % WIFISELFCHECKUPDATE == (WIFISELFCHECKUPDATE-1))
		{
			#ifdef  SUPPORT_WIFI_NET_FUNCTION
			SetWiFiUpdata(DPID_CHECKING_RESULT, SelfTestOK, 0xFF, 0, 0xFF, 0);
			#endif
		}
		else
		{
			if(IsLowPwrErr()==SET)
			{
				SetWiFiUpdata(DPID_BATTERY_STATE, BatteryLow, 0xFF, 0, 0xFF, 0);
			}else
			{
				SetWiFiUpdata(DPID_BATTERY_STATE, BatteryHig, 0xFF, 0, 0xFF, 0);
			}
		}
		
		#ifdef  SUPPORT_WIFI_NET_FUNCTION
		WiFiOpenCheck();
		#endif

		if(LifeDays % WIFIRECONNECTEDUPDATE==(WIFIRECONNECTEDUPDATE-1))
		{
			WifiAO.WifiCheckUpdateFlag = TRUE;
		}
	}
}

#ifdef WiFiUpdataNew
void WiFiOpenCheck(void)
{
	#if 0
	//WIFI Open Cheack
	if((mcu_get_wifi_work_state()==WIFI_CONN_CLOUD)||(GetCOState()==eCOed)||(GetSmokeState()==eSmokeState_Alarm))
	{
		WifiAO.WifiCONNFlag = TRUE;
	}else if(WifiAO.WifiPiredFlag)
	{
		WifiInit();
	}
	#endif
	if(WifiAO.WifiPiredFlag)
	{
		WifiInit();
	}
}
void SetWiFiUpdata(uint8_t dpid,uint16_t Value,uint8_t dpid1,uint16_t Value1,uint8_t dpid2,uint16_t Value2)
{
	#if 0
	uint8_t i;
	for(i=0;i<UpdataMax;i++)
	{
		if(UpdataWifi[i].DPID==0xFF)
		{
			UpdataWifi[i].DPID=dpid;
			UpdataWifi[i].data=Value;
			break;
		}
	}
	if(i>2)
	{
		UpdataWifi[0].DPID=dpid;
		UpdataWifi[0].data=Value;
	}
	#endif
	UpdataWifi[0].DPID=dpid;
	UpdataWifi[0].data=Value;
	UpdataWifi[1].DPID=dpid1;
	UpdataWifi[1].data=Value1;
	UpdataWifi[2].DPID=dpid2;
	UpdataWifi[2].data=Value2;
}

void ReSetWiFiUpdata(void)
{
	uint8_t i;
	for(i=0;i<UpdataMax;i++)
	{
		UpdataWifi[i].DPID=0xFF;
		UpdataWifi[i].data=0;
	}
}

#else

void Get_DpId_Data(uint8_t dpid,uint8_t value)
{
	WifiAO.WifiUpdateValue=value;
	WifiAO.WifiUpdateState = dpid;

	if((mcu_get_wifi_work_state()==WIFI_CONN_CLOUD)||(GetCOState()==eCOed)||(GetSmokeState()==eSmokeState_Alarm))
	{
		WifiAO.WifiCONNFlag = TRUE;
	}else if(WifiAO.WifiPiredFlag)
	{
		WifiInit();  
	}

	#if 0
	if(WifiAO.WifiPiredFlag)
	{
		WifiInit();  
	}
	#endif
}
void WIFIStateHandle(uint8_t WifiState)
{
	#ifdef SUPPORT_NET_FUNCTION_DEBUG
	printf_log("WifiState:%d\n",(int)WifiState);
	#endif

	switch(WifiState)
	{
		case DPID_CHECKING_RESULT:			
			mcu_dp_enum_update(DPID_CHECKING_RESULT,WifiAO.WifiUpdateValue);
			break;
		case DPID_SMOKE_SENSOR_STATE:
			mcu_dp_enum_update(DPID_SMOKE_SENSOR_STATE,WifiAO.WifiUpdateValue); 
			break;
		case DPID_FAULT:
			mcu_dp_fault_update(DPID_FAULT,WifiAO.WifiUpdateValue);
			break;
		case DPID_BATTERY_STATE:
			mcu_dp_enum_update(DPID_BATTERY_STATE,WifiAO.WifiUpdateValue);
			break;
		case DPID_MUFFLING:
			mcu_dp_bool_update(DPID_MUFFLING,WifiAO.WifiUpdateValue);
			break;
		case DPID_CO_STATE:
			mcu_dp_enum_update_co(DPID_CO_STATE,WifiAO.WifiUpdateValue,GetCOPpm());
			break;
		case DPID_CO_VALUE:
			mcu_dp_value_update(DPID_CO_VALUE,GetCOPpm());
		break;
		default:
			break;
	}
	SetSysSleepFlag(eActiveFlagWifiTimeout);
	TmrInsertTimer(GetTmrHd(),TMR_PERIOD(1000),PollingWifiTimeout,(void*)(WifiAO.WifiUpdateState));
	WifiAO.WifiUpdateState = 0xff;
	WifiAO.WifiUpdateValue = 0xff;
}

#endif

void CheckWiFi(SystemApiTag *me)
{
	uint8_t i = 0;
	uint8_t state;
	uint8_t PA7State;

	WifiVarInit();
	PA7State = GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN);

	printf("WifiFactory:%d \n",WifiAO.WifiFactory); 
	if(WifiAO.WifiFactory != FACTRORY_CHECKWIFI_FLAG)//Not Check WIFI
	{
		if(0==PA7State)// WIFI TEST MODE
		{
			if(0==GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN))// WIFI TEST MODE
			{
				WifiInit();
				DelayMs(1000);
				G_LED_TURN_OFF();
				SystemPolling();
				DelayMs(10);
				printf("send Test \n");
				state = WIFI_SATE_UNKNOW;	
				
				TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), NetConnetPolling));
				ResetSysSleepFlag(eActiveFlagNetConnetTimeout);
				while(1)
				{
					IWDG_ReloadCounter();
					SystemPolling();
					wifi_uart_service();
					state = mcu_get_wifi_work_state();
					if(((SMART_CONFIG_STATE==state)||(AP_STATE==state))&&(i<255))				
					{
						i++;
						if(i>=255)
						{
							mcu_start_wifitest();//延时发送测试
						}else
						{
							DelayMs(8);//10ms
						}
					}
					else if((WIFI_NOT_CONNECTED==state)||(WIFI_CONNECTED==state))
					{
						break;
					}
				} 
			}
		}
	}
	
	if(0==PA7State)// FOR PWR IDLE
	{
		DelayMs(10);//10ms
		if(0==GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN))// FOR PWR IDLE
		{
			i = 0;
			RtcWakeUpInit(RTC_SLEEP_TIME_1s);
			while((0==GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN))&&(i<15))
			{
				IWDG_ReloadCounter();
				G_LED_TURN_ON();
				me->PowerConsumptionCheckFlag = SET;
				EnterActiveHaltMode();
				i++;
			}
			ResetSysSleepFlag(eActiveFlagNetConnetTimeout);
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), NetConnetPolling));
			WifiDeinit();
			RtcWakeUpInit(RTC_SLEEP_TIME_10s);
		}
	}
}
void WiFiRSSIDisPlay(void)
{
	#define WIFILEVEL0 60
	#define WIFILEVEL1 75
	#define WIFILEVEL2 90
	
	LcdDisplayPoint(eLcdPointOFF, LCD_WIFI_COM0, LCD_WIFI_SEG);
	LcdDisplayPoint(eLcdPointOFF, LCD_WIFI_COM1, LCD_WIFI_SEG);
	LcdDisplayPoint(eLcdPointOFF, LCD_WIFI_COM2, LCD_WIFI_SEG);
	
	if(WifiAO.WifiRSSI>=WIFILEVEL2)
	{
		LcdDisplayPoint(eLcdPointON, LCD_WIFI_COM0, LCD_WIFI_SEG);
		LcdDisplayPoint(eLcdPointON, LCD_WIFI_COM1, LCD_WIFI_SEG);
		LcdDisplayPoint(eLcdPointON, LCD_WIFI_COM2, LCD_WIFI_SEG);
	}else if(WifiAO.WifiRSSI>=WIFILEVEL1)
	{
		LcdDisplayPoint(eLcdPointON, LCD_WIFI_COM0, LCD_WIFI_SEG);
		LcdDisplayPoint(eLcdPointON, LCD_WIFI_COM1, LCD_WIFI_SEG);
	}
	else if(WifiAO.WifiRSSI>=WIFILEVEL0)
	{
		LcdDisplayPoint(eLcdPointON, LCD_WIFI_COM0, LCD_WIFI_SEG);
	}
}
#if 0
void CheckWiFi(SystemApiTag *me)
{
	uint8_t i = 0;
	uint8_t state;
	uint8_t PA7State;
	
	
	WifiVarInit();
	PA7State = GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN);
	
	if((FLASH_ReadByte(FLASH_DATA_EEPROM_ADDR_CALI_FLAG) == CALI_FLAG_VALUE)&&(FlashGetCOState() == CO_CALI_STATE))
	{
		WifiInit();
		DelayMs(1000);
		G_LED_TURN_OFF();
		SystemPolling();
		printf("WifiFactory:%d \n",WifiAO.WifiFactory);	
		if(WifiAO.WifiFactory!= FACTRORY_CHECKWIFI_FLAG)
		{
			
			if(0==PA7State)// WIFI TEST MODE
			{
				DelayMs(10);
				if(0==GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN))// WIFI TEST MODE
				{
					printf("send Test \n");
					state = WIFI_SATE_UNKNOW;	
					while(1)
					{
						IWDG_ReloadCounter();
						SystemPolling();
						wifi_uart_service();
						state = mcu_get_wifi_work_state();	
						if(((SMART_CONFIG_STATE==state)||(AP_STATE==state))&&(i<255))				
						{
							i++;
							if(i>=255)
							{
								mcu_start_wifitest();//延时发送测�?
							}else
							{
								DelayMs(8);//10ms
							}
						}
						else if((WIFI_NOT_CONNECTED==state)||(WIFI_CONNECTED==state))
						{
							break;
						}
					} 
				}
			}
		}

		if(0==PA7State)// FOR PWR IDLE
		{
			DelayMs(10);//10ms
			if(0==GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN))// FOR PWR IDLE
			{
				i = 0;
				RtcWakeUpInit(RTC_SLEEP_TIME_1s);
				while((0==GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN))&&(i<15))
				{
					IWDG_ReloadCounter();
					G_LED_TURN_ON();
					me->PowerConsumptionCheckFlag = SET;
					EnterActiveHaltMode();
					i++;
				}
				ResetSysSleepFlag(eActiveFlagNetConnetTimeout);
				TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(), NetConnetPolling));
				WifiDeinit();
				RtcWakeUpInit(RTC_SLEEP_TIME_10s);
			}
		}
	}
}
#endif
#endif
