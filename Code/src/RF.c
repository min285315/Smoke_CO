
#include "SceneInstance.h"
#include "module.h"




#ifdef SUPPORT_RF_NET_FUNCTION

/***************************************************宏定�?***************************************/


//[0xAF][MsgType][MsgData1][MsgData2][Reserve] [Crc]
//  1     	1        1     	 	1       1        1  
//total len = 6











/***************************************************变量******************************************/




 unsigned char volatile UartQueueBuf[UartQueueLimit] = {0};	
 unsigned char UartReadBuf[UartBufLimit] = {0};				
 unsigned char UartSendBuf[UartBufLimit] = {0};	

static UartSendDataTag SendDataTag = {eUartMsgNull, 0, 0};

volatile  unsigned char *UartQueueIn	= (unsigned char *)UartQueueBuf; 
volatile  unsigned char *UartQueueOut   = (unsigned char *)UartQueueBuf; 

NetTag	NetAO;







static QEvent NetEvt = {.sig = Q_eSystemEventIdle,};	



/***************************************************函数******************************************/





/******************************************************************************
**Description:			NetWakeUPIn  wakeup rf															
** Parameters:                   none     												
**     Return:   				  none													
*******************************************************************************/

static void NetWakeUPIn(void)
{

	GPIO_Init( WAKEUP_PORT, WAKEUP_PIN ,GPIO_Mode_In_PU_IT);            //按键初始�?
	EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);            //设置外部中断1 为下降沿有效
	EXTI_ClearITPendingBit(EXTI_IT_Pin7);                               //清除中断标志�?

}


/******************************************************************************
**Description:		NetInit		init rf														
** Parameters:        none                												
**     Return:   		none															
*******************************************************************************/
void NetInit(void)
{
	NetWakeUPIn();
	NetAO.State = NetConnect;
	NetAO.NetDisconnectTime = 0;
	NetAO.NetTestLedFlickerNum = 0;
	NetAO.ProType = eProductType_XP01_Smart;
	NetAO.DevType = eTypeSmokeAndCo;

	UartQueueIn	 = (unsigned char *)UartQueueBuf;
	UartQueueOut = (unsigned char *)UartQueueBuf;
	
}


/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

static unsigned char UartGetQueueTotalData(void)
{
	if(UartQueueIn != UartQueueOut)
		return 1;
	else
		
		return 0;
}


/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/
static unsigned char UartQueueReadByte(void)
{
	unsigned char value = 0;
	//static uint8_t i =0;
	if(UartQueueOut != UartQueueIn)
	{
		
		if(UartQueueOut >= (unsigned char *)(UartQueueBuf + sizeof(UartQueueBuf)))
		{		
			UartQueueOut = (unsigned char *)(UartQueueBuf);

		}
		value = *UartQueueOut ++; 		
	}

	return value;
}

/**********************************************************************/
//Description:		GetCrc8  												  
//Parameters:                        										  
//Return:   											
//Date:  
//author:		    quanwu.xu
/**********************************************************************/
static uint8_t GetCrc8(uint8_t *buf, uint8_t len)
{
	int i;
	char crc = 0;
 	while(len--!=0)
	{
		for (i = 0x80; i != 0; i /= 2)		
		{
			if ((crc & 0x80) != 0)		
			{
				crc *= 2;
				crc ^= 0x07;
			} 
			else
			{
				crc *= 2;
			}

			if ((*buf & i) != 0)
			{
				crc ^= 0x07;
			}
		}
		buf++;
	}
	return crc;
}



/*****************************************************************************
函数名称 : my_memset
功能描述 : 把src所指内存区域的前count个字节设置成字符c
输入参数 : src:源地址
           ch:设置字符
           count:设置数据长度
返回参数 : src:数据处理完后的源地址
*****************************************************************************/

void *my_memset(void *src,unsigned char ch,unsigned short count)
{
  unsigned char *tmp = (unsigned char *)src;
  
  if(src == NULL)
  {
    return NULL;
  }
  
  while(count --)
  {
    *tmp++ = ch;
  }
  
  return src;
}

/**********************************************************************/
//Description:		Memcpy()															  
//Parameters:                        												  
//Return:   																  
//Date			2019-01-12
//Author:		quanwu.xu
/**********************************************************************/
static void* Memcpy(void* dest, void* src, uint8_t Length)
{
	int i;
	char *d = (char *)dest, *s = (char *)src;

	if((dest == NULL) ||(src == NULL) || (Length == 0))
	{
		return NULL;
	}
	
	for (i=0; i<Length; i++) 
	{	
		d[i] = s[i];
	}
	return dest;
}


/*****************************************************************************
函数名称 : set_wifi_uart_byte
功能描述 : 写uart字节
输入参数 : dest:缓存区其实地址;
           byte:写入字节�?
返回参数 : 写入完成后的总长�?
*****************************************************************************/
static uint8_t SetUartTxBufByte(uint8_t dest, uint8_t byte)
{
	unsigned char *obj = (unsigned char *)UartSendBuf + UART_MSG_DATA_START + dest;

	*obj = byte;
	dest += 1;

	return dest;
}
/*****************************************************************************
函数名称 : SetUartTxBufBuffer
功能描述 : 写uart_buffer
输入参数 : dest:目标地址
           src:源地址
           len:数据长度
返回参数 : �?
*****************************************************************************/
uint8_t SetUartTxBufBuffer(uint8_t dest, uint8_t *src, uint8_t len)
{
	unsigned char *obj = (unsigned char *)UartSendBuf + UART_MSG_DATA_START + dest;

	Memcpy(obj,src,len);

	dest += len;
	return dest;
}


/**********************************************************************/
//Description:	UartSendFrame()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void UartSendFrame(uint8_t *SendBuf, uint8_t len)
{
	uint8_t i = 0;

	if((NULL == SendBuf) || (0 == len))
	{
		return;
	}

	for(i=0; i<len; i++)
	{
		UartSendData(SendBuf[i]);
	}
}


/**********************************************************************/
//Description:	UartWriteFrame()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
static void UartWriteFrame(uint8_t UartCmd, uint8_t length)
{
	unsigned char check_sum = 0;

	UartSendBuf[UART_MSG_HEAD] = 0xAF;
	UartSendBuf[UART_MSG_TYPE] = UartCmd;
	UartSendBuf[UART_MSG_LENGTH] = length;

	length += UART_MSG_FIXED_LENGTH;
	check_sum = GetCrc8((unsigned char *)UartSendBuf, length - 1);
	UartSendBuf[length - 1] = check_sum;
	UartSendFrame(UartSendBuf, length);
}


/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void NetMain(void)
{
	static uint16_t rx_in = 0;
	uint8_t offset = 0;			
	uint8_t CharValue = 0;
	uint8_t rx_value_len = 0;	

	ResetSysSleepFlag(eActiveFlagNet);	
	while((rx_in < sizeof(UartReadBuf)) && (UartGetQueueTotalData() > 0))
	{
		CharValue = UartQueueReadByte();
		UartReadBuf[rx_in ++] = CharValue;//UartQueueReadByte();
	}

	if(rx_in < UART_MSG_FIXED_LENGTH)
	{
		return;
	}

	//check real  data 
	while((rx_in - offset) > UART_MSG_FIXED_LENGTH)
	{
		if(UartReadBuf[offset + UART_MSG_HEAD] != UART_TX_FRAME_HEADER)
		{
			offset ++;	
			continue;
		}

		//rx_value_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] * 0x100 + wifi_uart_rx_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD;
		rx_value_len = UartReadBuf[offset + UART_MSG_LENGTH] + UART_MSG_FIXED_LENGTH;

		if(rx_value_len > UART_MSG_MAX_DATA_LENGTH)
		{
			offset += UART_MSG_LENGTH;
		}

		//data not receive over
		if((rx_in - offset) < rx_value_len)
		{
			break;
		}
#if 0
		printf("Rx: = %d,offset=%d\n",(int)rx_in,(int)offset);
		for(int i=0; i<6; i++)
		{
			printf("%x ",(int)UartReadBuf[offset + i]);
		}
		printf("\n");
#endif
		//数据接收完成,and compare check sum 
		if(GetCrc8((unsigned char *)UartReadBuf + offset, rx_value_len - 1) == UartReadBuf[offset + rx_value_len - 1])
		{
			//printf("Crc is ok\n");
			Net_data_handle(offset);
		}
		else
		{
			printf("Crc Err\n");
		}

		//offset is right start position
		offset += rx_value_len;
	}//end while
	
	rx_in -= offset;
	//copy remain data to first position
	if(rx_in > 0)
	{
		Memcpy(UartReadBuf, UartReadBuf + offset, rx_in);
	}	

	
}



/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void NetTestTimeout(TmrHdl Handle, eTmrCmd eCmd)
{	
	uint16_t state = (uint16_t)TmrGetCtx(Handle);

	switch (eCmd)
	{
		case eTimeOut:
		NetEvt.sig = eNETEventTimeout;
		SystemPostFifo(&NetEvt);
		ResetSysSleepFlag(eActiveFlagNetTimeOut);
		TmrDeleteTimer(Handle);
	
		break;
		
		default:
		break;
	}			
}

static void NetWakeUPOut(uint8_t Flag)
{
	GPIO_Init(WAKEUP_PORT, WAKEUP_PIN, GPIO_Mode_Out_PP_Low_Slow);
	if(Flag)
	{
		GPIO_SetBits(WAKEUP_PORT, WAKEUP_PIN);
	}else
	{
		GPIO_ResetBits(WAKEUP_PORT, WAKEUP_PIN);
	}
}

static void PollingNetTimeout(TmrHdl Handle, eTmrCmd eCmd)
{
	uint16_t state = (uint16_t)TmrGetCtx(Handle);
	static	uint8_t Num = 0;
	//printf("state= 0x%x,Num =%d,eCmd=%d\n",state,Num,eCmd);
	switch (eCmd)
	{
		case eTimeOut:
		switch (state)
		{
			case 0:
				Num = 0;	
				TmrDeleteTimer(Handle);
			break;
			
			case 1:
				if (Num>=3)
				{
					Num = 0;
					TmrDeleteTimer(Handle);	
				}else
				{
					UartSendReMsg(SendDataTag.MsgType, SendDataTag.MsgLevel1, SendDataTag.MsgData1, SendDataTag.MsgData2);
					Num++;
				}
			break;
			
			default:				
			break;
		}
		break;
	case eInsertTimer:		
		SetSysSleepFlag(eActiveFlagNetCMDTimeOut);
			break;
		case eDelTimer:	
			Num = 0;
		ResetSysSleepFlag(eActiveFlagNetCMDTimeOut);
			break;	
	default:
		break;

	}		
		
}


/**********************************************************************/
//Description:	UartSendAck()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
bool UartSendAck(uint8_t UartMsgType, uint8_t TypeAck,uint8_t UartMsgLevel)
{
	uint8_t Length = 0;
	Length = SetUartTxBufByte(Length,TypeAck);
	Length = SetUartTxBufByte(Length,UartMsgLevel);
	UartWriteFrame(UartMsgType, Length);
	
	return TRUE;
}
void UartSendReMsg(eUartMsgType UartMsgType, uint8_t MsgLevel1, uint8_t Data1, uint8_t Data2)
{

	uint8_t Length = 0;

	SendDataTag.MsgType = UartMsgType;
	SendDataTag.MsgLevel1 = MsgLevel1;
	SendDataTag.MsgData1 = Data1;
	SendDataTag.MsgData2 = Data2;

	NetWakeUPOut(0);
	DelayMs(3);

	Length = SetUartTxBufByte(Length,MsgLevel1);
	if(UartMsgType == eUartMsgPair)
	{
		Length = SetUartTxBufByte(Length,(uint8_t)NetAO.DevType);	
		Length = SetUartTxBufByte(Length,(uint8_t)NetAO.ProType);
	}
	
	if((MsgLevel1 == UartFirmAlarmMsgCoAlarm)||(MsgLevel1 == UartFirmAlarmMsgUpdateCoPpm))
	{
		Length = SetUartTxBufByte(Length,Data1);
		Length = SetUartTxBufByte(Length,Data2);
	}
	else if(Data1 != 0)
	{
		Length = SetUartTxBufByte(Length,Data1);
	}

	UartWriteFrame(UartMsgType, Length);//发送数�?

	NetWakeUPIn();
}

/**********************************************************************/
//Description:	UartSendAck()									
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void UartSendMsg(eUartMsgType UartMsgType, uint8_t MsgLevel1, uint8_t Data1, uint8_t Data2)
{
	UartSendReMsg(UartMsgType,MsgLevel1,Data1,Data2);
	TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000),PollingNetTimeout, (void*)1);	

}








/**********************************************************************/
//Description:		NetLedFlicker   														  
//Parameters:         Time ：led on time               												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
void PollingNetLedFlicker(TmrHdl Handle, eTmrCmd eCmd)
{
	uint16_t state = (uint16_t)TmrGetCtx(Handle);
	
	switch (eCmd)
	{
		case eTimeOut:
			LedPollingHandle(LED_TYPE_RED,state);
			break;
		
		default:
			break;
	}		
}



void PollingNet_LedFlicker(TmrHdl Handle, eTmrCmd eCmd)
{
	uint16_t state = (uint16_t)TmrGetCtx(Handle);
	
	switch (eCmd)
	{
		case eTimeOut:
			LedPollingHandle(LED_TYPE_RED,state);
			SetSysSleepFlag(eActiveFlagNetKeyTestGREENLED);
   		 	TmrInsertTimer(GetTmrHd(), TMR_PERIOD(210), PollingLedGreen, ((void*)200));//
			break;
		
		default:
			break;
	}		
}

/**********************************************************************/
//Description:		NetBeepFlicker   														  
//Parameters:         Time ：led on time               												  
//Return:   											
//Date:  
//author:		    song
/**********************************************************************/
void PollingNetBeepFlicker(TmrHdl Handle, eTmrCmd eCmd)
{
	uint16_t state = (uint16_t)TmrGetCtx(Handle);
	
	switch (eCmd)
	{
		case eTimeOut:
			BeepPollingHandle(200);
			break;
			
		default:
			break;
	}		
}



void NETFactoryMode(SystemApiTag  *me,QEvent *e)
{

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
			#ifdef DEBUG_NET
				printf("%s in,%d\n",__FUNCTION__,__LINE__);
			#endif
			
			UartSendMsg(eUartMsgPair, UartMsgPairFactoryMode, 0,0);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(500),PollingNetLedFlicker, (void*)200);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(10000),NetTestTimeout, (void*)0);
			SetSysSleepFlag(eActiveFlagNetTimeOut);
			break;
			
		case Q_EXIT_SIG:
			#ifdef DEBUG_NET
				printf("%s out,%d\n",__FUNCTION__,__LINE__);			
			#endif
			
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetLedFlicker));
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),NetTestTimeout));	
			ResetSysSleepFlag(eActiveFlagNetTimeOut);
			break;
	   case eNETEventPairedOK:
	   		BeepPollingHandle(200);
			LedPollingHandle(LED_TYPE_GREEN,200);
	   		SetNetConfig (ePairedIntranet);
	       #ifdef SUPPORT_SMOKE_FUNCTION
			Q_TRAN(&SysSmokeCailModeHandleEvent);	  // init over ,tran to factory mode  
			#else
			#ifdef SUPPORT_CO_FUNCTION
			Q_TRAN(&SysCoCailModeHandleEvent); 		// init over ,tran to factory mode
			#else
			Q_TRAN(&SystemStartHandle); 		// init 
			#endif
			#endif
	   	break;
		
		case eNETEventTimeout:		
			#ifdef SUPPORT_SMOKE_FUNCTION
			Q_TRAN(&SysSmokeCailModeHandleEvent);	  // init over ,tran to factory mode  
			#else
			#ifdef SUPPORT_CO_FUNCTION
			Q_TRAN(&SysCoCailModeHandleEvent); 		// init over ,tran to factory mode
			#else
			Q_TRAN(&SystemStartHandle); 		// init 
			#endif	
			#endif
			break;

		default:
			break;
	}

}

void NetPairingHubMode(SystemApiTag  *me,QEvent *e)
{	
	switch (e->sig)
	{	
		case Q_ENTRY_SIG:
			#ifdef Debug_Test
				printf("%s in,%d\n",__FUNCTION__,__LINE__);
			#endif		
			BeepPollingHandle(100);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000),PollingNetLedFlicker, (void*)500);
			//to be done send pair command
			UartSendMsg(eUartMsgPair, UartMsgPairHubMode, 0, 0);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(60000),NetTestTimeout, (void*)0);
			SetSysSleepFlag(eActiveFlagNetTimeOut);
		break;
			
		case Q_EXIT_SIG:
			#ifdef Debug_Test
				printf("%s out,%d\n",__FUNCTION__,__LINE__);
			#endif
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetLedFlicker));
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),NetTestTimeout));	
			ResetSysSleepFlag(eActiveFlagNetTimeOut);
			break;

		
		case eNETEventPairedOK:
			SetNetConfig (ePairedHub);
			BeepPollingHandle(200);
			Q_TRAN(SysIdleModeHandleEvent);
			break;
			
		case Q_eKeyEventKey1Clik:
		case eNETEventTimeout:
			UartSendMsg(eUartMsgPair, UartMsgPairStop, 0, 0);
			Q_TRAN(SysIdleModeHandleEvent);
		break;	
	}
}


void NetTxPairMode(SystemApiTag  *me,QEvent *e)
{	
	switch (e->sig)
	{	

		case Q_ENTRY_SIG:
			#ifdef DEBUG_NET
				printf("%s in,%d\n",__FUNCTION__,__LINE__);
			#endif
			
			BeepPollingHandle(100);
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(500),PollingNetLedFlicker, (void*)100);
		    //to be done send pair command
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(60000),NetTestTimeout, (void*)0);
			SetSysSleepFlag(eActiveFlagNetTimeOut);
			UartSendMsg(eUartMsgPair, UartMsgPairSlaverMode, 0, 0);
		break;
			
		case Q_EXIT_SIG:
			#ifdef DEBUG_NET
				printf("%s out,%d\n",__FUNCTION__,__LINE__);
			#endif
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetLedFlicker));
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),NetTestTimeout));	
			ResetSysSleepFlag(eActiveFlagNetTimeOut);
		break;
		case eNETEventPairedOK:		
			#ifdef DEBUG_NET
				printf("%s ,%d\n",__FUNCTION__,__LINE__);
			#endif
			SetNetConfig(ePairedIntranet);
			BeepPollingHandle(200);
			Q_TRAN(SysIdleModeHandleEvent);
		break;	
		
		case Q_eKeyEventKey0Clik:	
		case eNETEventTimeout:
			UartSendMsg(eUartMsgPair, UartMsgPairStop, 0, 0);
			Q_TRAN(SysIdleModeHandleEvent);			
		break;	
		
		
	}
}


/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void NetRxPairMode(SystemApiTag  *me,QEvent *e)
{	
	switch (e->sig)
	{	
		case Q_ENTRY_SIG:
			#ifdef DEBUG_NET
				printf("%s in,%d\n",__FUNCTION__,__LINE__);
			#endif	
			
			BeepPollingHandle(100);
			LedPollingHandle(LED_TYPE_RED,200);
			UartSendMsg(eUartMsgPair, UartMsgPairMasterMode, 0, 0);
	        TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000),PollingNetLedFlicker, (void*)500);
		    //to be done send pair command
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(60000),NetTestTimeout, (void*)0);
			SetSysSleepFlag(eActiveFlagNetTimeOut);
			break;
			
		case Q_EXIT_SIG:
			#ifdef DEBUG_NET
			{
				printf("%s out,%d\n",__FUNCTION__,__LINE__);
			}
			#endif
			
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetLedFlicker));
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),NetTestTimeout));	
			ResetSysSleepFlag(eActiveFlagNetTimeOut);
			break;
		
		case eNETEventPairedOK:		
			#ifdef DEBUG_NET			
			{
				printf("%s ,%d\n",__FUNCTION__,__LINE__);
			}
			#endif
			SetNetConfig(ePairedIntranet);
			BeepPollingHandle(200);
			Q_TRAN(SysIdleModeHandleEvent);
		break;
			
		case Q_eKeyEventKey0Clik:
		case eNETEventTimeout:
			UartSendMsg(eUartMsgPair, UartMsgPairStop, 0, 0);
			Q_TRAN(SysIdleModeHandleEvent);			
			break;	
	
	}
}

/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void NetExitPairMode(SystemApiTag  *me,QEvent *e)
{	
	switch (e->sig)
	{	
		case Q_ENTRY_SIG:
			#ifdef DEBUG_NET
				printf("%s in,%d\n",__FUNCTION__,__LINE__);
			#endif	
			
			BeepPollingHandle(100);
			LedPollingHandle(LED_TYPE_RED,100);	        
			TmrInsertTimer(GetTmrHd(), TMR_PERIOD(6000),NetTestTimeout, (void*)0);
			SetSysSleepFlag(eActiveFlagNetTimeOut);
			break;
			
		case Q_EXIT_SIG:
			#ifdef DEBUG_NET
				printf("%s out,%d\n",__FUNCTION__,__LINE__);
			#endif			
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),NetTestTimeout));	
			ResetSysSleepFlag(eActiveFlagNetTimeOut);
		break;
			
		case eNETEventTimeout://4击后6s内后没有检测到长按则退出该模式
			Q_TRAN(SysIdleModeHandleEvent);	
		break;
		
		case Q_eKeyEventKey0LongClik:	// for	CLODR   RF
			UartSendMsg(eUartMsgPair, UartMsgPairResetId, 0,0);

		break;
		
		case eNETEventExitPairedOK:
			SetNetConfig(ePairedNull);	
			LedPollingHandle(LED_TYPE_GREEN, 500);
			BeepPollingHandle(500);
			Q_TRAN(SysIdleModeHandleEvent);	
		break;
	
	}
}




/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void NetSendTestMode(SystemApiTag  *me,QEvent *e)
{	
	switch (e->sig)
	{
	
	    case Q_ENTRY_SIG:
		    #ifdef SUPPORT_LCD_FUNCTION	
			LCD_Control(eLcdON);
			//LCD_BackLightControl(eLcdBackLightON);//
			BatteryLcdDisplay();
			LCD_DsiplayCoFlg(eLcdON);
			LCD_DisplayNum(0);
		    #endif
		    TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000),PollingNetLedFlicker, (void*)200);
		    //NetSendCmd(NetMsgTest);
		    TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000),PollingNetBeepFlicker, NULL);
		    TmrInsertTimer(GetTmrHd(), TMR_PERIOD(120000),NetTestTimeout, NULL);
		    SetSysSleepFlag(eActiveFlagNetTimeOut);
			
			UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgTest, DevTestStart, 0);
		  
		    #ifdef DEBUG_NET		   
			{
				printf("%s in,%d\n",__FUNCTION__,__LINE__);
			}
		    #endif
		    break;
			
		case Q_EXIT_SIG:
			#ifdef DEBUG_NET				
			{
				printf("%s out,%d\n",__FUNCTION__,__LINE__);
			}
			#endif
			UartSendMsg(eUartMsgFireAlarm, UartFirmAlarmMsgTest, DevTestStop, 0);
			
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetLedFlicker));
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetBeepFlicker));
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),NetTestTimeout));
			ResetSysSleepFlag(eActiveFlagNetTimeOut);
			break;
			
		case eNETEventAck:
			TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetTimeout));	
			break;
		
		case Q_eKeyEventKey1LongClikRelease:
		case eNETEventTimeout:
			Q_TRAN(SysIdleModeHandleEvent);			
			break;	
	}
}


/******************************************************************************
**Description:																		
** Parameters:                        												
**     Return:   																	
*******************************************************************************/

void NetTestMode(SystemApiTag  *me,QEvent *e)
{	
	switch (e->sig)
	{
	
	case Q_ENTRY_SIG:
		#ifdef SUPPORT_LCD_FUNCTION
		LCD_Control(eLcdON);
		BatteryLcdDisplay();
		LCD_DsiplayCoFlg(eLcdON);
		LCD_DisplayNum(0);
		#endif
	
		#ifdef DEBUG_NET
		
		{
			printf("%s in,%d\n",__FUNCTION__,__LINE__);
		}
		#endif
		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000),PollingNet_LedFlicker, (void*)200);
		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000),PollingNetBeepFlicker, NULL);
		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(120000),NetTestTimeout, (void*)0);
		SetSysSleepFlag(eActiveFlagNetTimeOut);
		break;
			
	case Q_EXIT_SIG:
		#ifdef DEBUG_NET
		
		{
			printf("%s out,%d\n",__FUNCTION__,__LINE__);
		}
		#endif
		TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNet_LedFlicker));
		TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetBeepFlicker));
		TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),NetTestTimeout));
		ResetSysSleepFlag(eActiveFlagNetTimeOut);
		break;
		
	case Q_eKeyEventKey1Clik:
	case eNETEventTestFinished:
	case eNETEventTimeout:
		Q_TRAN(SysIdleModeHandleEvent);			
		break;	
	}
}

/**********************************************************************/
//Description:	UartPairMsgHandle()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
static void UartPairMsgHandle(uint8_t PairStates, uint8_t MsgData)
{
	if(PairStates == UartMsgPairAck)
	{
		TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetTimeout));	
		if(MsgData == UartMsgPairResetId)								//RestID
		{
			NetEvt.sig = eNETEventExitPairedOK;
			SystemPostFifo(&NetEvt);
		}
	}
	else
	{
		UartSendAck(eUartMsgPair, UartMsgPairAck, PairStates);

		switch (PairStates)
		{
				
			case UartMsgPairSuccess:
				NetEvt.sig = eNETEventPairedOK;
				
				break;

			case UartMsgPairFail:
				break;	

			default:
				break;
		}
		SystemPostFifo(&NetEvt);
	}
}
/**********************************************************************/
//Description:	UartSmokeAndCoMsgHandle()											
//Parameters:                        									
//Return:   															
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
static void UartSmokeAndCoMsgHandle(uint8_t SmokeMsgType, uint8_t MsgData)
{
	if(SmokeMsgType == UartFirmAlarmMsgAck)
	{
		TmrDeleteTimer(TmrGetHandleByEvent(GetTmrHd(),PollingNetTimeout));	
	}
	else
	{
		UartSendAck(eUartMsgFireAlarm, UartFirmAlarmMsgAck, SmokeMsgType);	
		
		switch (SmokeMsgType)
		{
			case UartFirmAlarmMsgMute:
				NetEvt.sig = eNETEventMute;
				break;

			case UartFirmAlarmMsgCancelMute:
				NetEvt.sig = eNETEventCancelMute;
				break;

			case UartFirmAlarmMsgTest:
				if(MsgData == DevTestStart)
				{
					if(SystemGetNetMode()==ePairedHub)
					{
						NetEvt.sig = Q_eKeyEventKey1Clik; 
					}
					else 
					{
					    NetEvt.sig = eNETEventTest;	
						
					}
					
				}
				else if(MsgData == DevTestStop)
				{
					NetEvt.sig = eNETEventTestFinished;
				}
				 
				break;
				
			case UartFirmAlarmMsgSmokeAlarm:
			case UartFirmAlarmMsgHeatAlarm:				
			case UartFirmAlarmMsgGasAlarm:
				NetEvt.sig = eNETEventSmokeAlarm;
				break;
				
			case UartFirmAlarmMsgCoAlarm:
				NetEvt.sig = eNETEventCoAlarm;	
				break;
				
			case UartFirmAlarmMsgCancelAlarm:
				NetEvt.sig = eNETEventCancleAlarm;
				break;

			default:
				break;
		}
		SystemPostFifo(&NetEvt);
	}
}


/*****************************************************************************
函数名称 : Net_data_handle
功能描述 : RF串口数据处理
输入参数 : offset:解码数据获得的偏移量
返回参数 : 
*****************************************************************************/

void Net_data_handle(unsigned short offset)
{
	//uint8_t DataBuf[UART_MSG_MAX_DATA_LENGTH] = {0};
#ifdef SUPPORT_NET_FUNCTION_DEBUG	
	uint8_t i, PrintLen;
	uint8_t UartMsgDataLength = UartReadBuf[offset + UART_MSG_LENGTH];
#endif	
	eUartMsgType UartMsgType = (eUartMsgType)UartReadBuf[offset + UART_MSG_TYPE];	
	uint8_t MsgData1 = UartReadBuf[offset + UART_MSG_DATA_START];
	uint8_t MsgData2 = UartReadBuf[offset + UART_MSG_DATA_START + 1];
	
	//Memcpy((void *)DataBuf, (void *)&UartReadBuf[offset + UART_MSG_DATA_START], UartMsgDataLength);

	switch(UartMsgType)
	{
		case eUartMsgPair:
			UartPairMsgHandle(MsgData1, MsgData2);
			break;


		case eUartMsgFireAlarm:
			UartSmokeAndCoMsgHandle(MsgData1, MsgData2);
			break;
		
		default:
		break;
	}
	
#ifdef SUPPORT_NET_FUNCTION_DEBUG
		PrintLen = UartMsgDataLength + UART_MSG_FIXED_LENGTH;
		printf("Uart Rx: ");
		for(i=0; i<PrintLen; i++)
		{
			printf("%x ", UartReadBuf[offset + i]); 
		}
		printf("\n");	
		
		printf("Type=%d, Data: ", UartMsgType);
		for(i=0; i<UartMsgDataLength; i++)
		{
			printf("%x ", DataBuf[i]);	
		}
		printf("\n");
	
#endif



}







#endif

