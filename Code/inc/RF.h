

#ifndef _RF_H_
#define _RF_H_
#include "main.h"

#ifndef __SUPPORT_RF_FUNCTION__
#define __SUPPORT_RF_FUNCTION__

/***************************************************宏定义****************************************/
#define WAKEUP_PORT                 GPIOE
#define WAKEUP_PIN                  GPIO_Pin_7
#define WAKEUP_EXTI_PIN             EXTI_Pin_7

#define UART_TX_FRAME_HEADER   		(0xAF)
#define UART_MSG_FIXED_LENGTH		(0x04)
#define UART_MSG_MAX_DATA_LENGTH	(0x0F)

#define UART_MSG_HEAD		0
#define UART_MSG_TYPE		1
#define UART_MSG_LENGTH		2
#define UART_MSG_DATA_START	3

#define UART_MSG_DATA_Len	3


#define UartQueueLimit	((uint8_t)64)
#define UartBufLimit	((uint8_t)96)


//=============================================================================
//RF Cmd
//=============================================================================
//产品代号Production type
typedef enum _eProductType
{
	eProductType_NULL = 0x00,
	eProductType_Hub1,
	eProductType_Hub2,
	eProductType_Hub3,
	eProductType_Repeater1_Smart,
	eProductType_Repeater2_Smart,
	eProductType_Repeater3_Smart, 
	eProductType_SC01_Smart,
	eProductType_SC03_Smart,
	eProductType_SD01_Smart,
	eProductType_SD03_Smart,
	eProductType_CD01_Smart,
	eProductType_CD03_Smart,
	eProductType_CD14_Smart,
	eProductType_XC01_Smart,
	eProductType_XC02_Smart,
	eProductType_XC03_Smart,
	eProductType_XP01_Smart,			//17
	eProductType_XP01_S_Smart,
	eProductType_XP02_Smart,
	eProductType_XP02_S_Smart,			//20
	eProductType_XP03_Smart,
	eProductType_XP03_S_Smart,
	eProductType_XS01_Smart,
	eProductType_XS02_Smart,
	eProductType_XS03_Smart,			//25
	eProductType_HC01_Smart,
	eProductType_HC02_Smart,
	eProductType_HC03_Smart,
	eProductType_GC01_Smart,
	eProductType_GC02_Smart,			//30
	eProductType_GC03_Smart,
	eProductType_Heat01_Smart,
	eProductType_Heat02_Smart,
	eProductType_Heat03_Smart,
	eProductType_Gas01_Smart,
	eProductType_Gas02_Smart,
	eProductType_Gas03_Smart,
	eProductType_Water01_Smart,
	eProductType_Water02_Smart,
	eProductType_Water03_Smart,
	eProductType_Plug01_Smart,
	eProductType_Plug02_Smart,
	eProductType_Plug03_Smart,
	eProductType_Romote01_Smart,
	eProductType_Romote02_Smart,
	eProductType_Romote03_Smart,
	eProductType_PIR01_Smart,
	eProductType_PIR02_Smart,
	eProductType_PIR03_Smart,
	eProductType_Magnets01_Smart,
	eProductType_Magnets02_Smart,
	eProductType_Magnets03_Smart,
	eProductType_DriveWay01_Smart,
	eProductType_DriveWay02_Smart,
	eProductType_DriveWay03_Smart,
	eProductType_GlassAlarm01_Smart,
	eProductType_GlassAlarm02_Smart,
	eProductType_GlassAlarm03_Smart,
	eProductType_SOS01_Smart,
	eProductType_SOS02_Smart,
	eProductType_SOS03_Smart,
	eProductType_Actuator01_Smart,
	eProductType_Actuator02_Smart,
	eProductType_Actuator03_Smart,
	eProductType_KeyBoard01_Smart,
	eProductType_KeyBoard02_Smart,
	eProductType_KeyBoard03_Smart,
	eProductType_DoorBell01_Smart,
	eProductType_DoorBell02_Smart,
	eProductType_DoorBell03_Smart,
	eProductType_Horn01_Smart,
	eProductType_Horn02_Smart,
	eProductType_Horn03_Smart,
	eProductType_Camera01_Smart,
	eProductType_Camera02_Smart,
	eProductType_Camera03_Smart,
	eProductType_WeatherStation01_Smart,
	eProductType_WeatherStation02_Smart,
	eProductType_WeatherStation03_Smart,

	//Aegislink
	//eProductType_SC200_W_Smart,//80
}eProductType;


//设备类型
typedef enum _eRfDeviceType{
	eTypeNull = 0x00,
	eTypeHub,                    //Hub设备
	eTypeRepeater,               //中继器
	eTypeSmoke,                  //烟感报警器
	eTypeCo,                     //Co报警器
	eTypeHeat,					 //热报警器
	eTypeGas,					 //燃气
	eTypeSmokeAndCo,             //SC复合型报警器
	eTypeHeatAndCo,              //HC复合型报警器
	eTypeGasAndCo,               //GC复合报警器
	eTypeWaterSensor,            //水浸 
	eTypePlug,                   //插座
	eTypeRomote,                 //遥控器
	eTypePir,                    //Pir
	eTypeMagnet,                 //门窗磁
	eTypeDriveWay,               //车道报警器
	eTypeGlassBrokenAlarm,       //玻璃碎探测报警器
	eTypeSos,                    //求救器
	eTypeActuator,               //执行器
	eTypeKeyBoard,				 //键盘设备
	eTypeDoorBell,				 //门铃
	eTypeHorn,					 //喇叭
	eTypeCamera,				 //摄像头
	eTypeWeatherStation,		 //温湿计
}eRfDeviceType;


//一级消息类型
typedef enum _eUartMsgType{
	eUartMsgNull = 0x00,
	eUartMsgPair,
	eUartMsgRfStates,
	eUartMsgFireAlarm,
	eUartMsgWaterSensor,
	eUartMsgPlug,
	eUartMsgRomote,
	eUartMsgPir,
	eUartMsgMagnet,
	eUartMsgDriveWay,
	eUartMsgGlassBrokenAlarm,//10
	eUartMsgSos,
	eUartMsgActuator,
	eUartMsgKeyBoard,
	eUartMsgDoorBell,
	eUartMsgHorn,//15
	eUartMsgCamera,
	eUartMsgWeatherStation,
}eUartMsgType;

//Pair二级消息类型
#define UartMsgPairAck 				((uint8_t)0x01)
#define UartMsgPairResetId			((uint8_t)0x02)
#define UartMsgPairFactoryMode		((uint8_t)0x03)
#define UartMsgPairHubMode	 		((uint8_t)0x04)
#define UartMsgPairSlaverMode	 	((uint8_t)0x05)
#define UartMsgPairMasterMode	 	((uint8_t)0x06)
#define UartMsgPairSuccess			((uint8_t)0x07)
#define UartMsgPairFail				((uint8_t)0x08)
#define UartMsgPairStop				((uint8_t)0x09)


//Rf states二级消息类型
#define UartMsgRfStatesAck 		((uint8_t)0x01)
#define UartMsgRfStatesOnline	((uint8_t)0x02)
#define UartMsgRfStatesOffline	((uint8_t)0x03)

//FireAlarm二级消息类型
#define UartFirmAlarmMsgAck			((uint8_t)0x01)
#define UartFirmAlarmMsgMute		((uint8_t)0x02)
#define UartFirmAlarmMsgCancelMute	((uint8_t)0x03)
#define UartFirmAlarmMsgTest		((uint8_t)0x04)
#define UartFirmAlarmMsgFault		((uint8_t)0x05)
#define UartFirmAlarmMsgLowBat		((uint8_t)0x06)
#define UartFirmAlarmMsgSmokeAlarm	((uint8_t)0x07)
#define UartFirmAlarmMsgCoAlarm		((uint8_t)0x08)
#define UartFirmAlarmMsgHeatAlarm	((uint8_t)0x09)
#define UartFirmAlarmMsgGasAlarm	((uint8_t)0x0A)
#define UartFirmAlarmMsgCancelAlarm	((uint8_t)0x0B)
#define UartFirmAlarmMsgUpdateCoPpm ((uint8_t)0x0C)

//Bat	Level电池电量数据
//LowBat_Data Level电池电量
#define BatLevelLow			((uint8_t)0x01)
#define BatLevel3_1			((uint8_t)0x02)
#define BatLevel3_2			((uint8_t)0x03)
#define BatLevelFull		((uint8_t)0x04)


//Test_Data	测试结果
#define DevTestStart		((uint8_t)0x01)
#define DevTestStop			((uint8_t)0x02)
#define DevTestSuccess		((uint8_t)0x03)
#define DevTestFail			((uint8_t)0x04)
#define DevNormal			    ((uint8_t)0x06)

//Fault_Data 错误数据
#define DevFault				((uint8_t)0x01)
#define DevSmokeSenorFault		((uint8_t)0x02)
#define DevCoSenorFault			((uint8_t)0x03)
#define DevWaterSensorFault		((uint8_t)0x04)
#define DevTempFault			((uint8_t)0x05)
#define DevCancelFault			((uint8_t)0x06)


/***************************************************变量******************************************/


struct Net{
	uint8_t State;// Net state 
	uint32_t NetDisconnectTime;//RX offset 
	uint16_t NetTestLedFlickerNum;// add for test 
	FlagStatus RxFlag;// RX receive data
	uint8_t RxIn;// rx  buf in start 
	uint32_t RfDisconnectTime;//RX offset 
	eProductType ProType;
	eRfDeviceType DevType;
};
typedef struct Net NetTag;


typedef struct
{
	uint8_t UartMsgType;
	uint8_t	UartMsgData[3];
}UartRfStruct;

typedef struct _UartSendDataTag
{
	eUartMsgType MsgType;
	uint8_t MsgLevel1;
	uint8_t MsgData1;
	uint8_t MsgData2;
}UartSendDataTag;


extern NetTag	NetAO;
extern unsigned char UartReadBuf[UartBufLimit];			   
extern unsigned char UartSendBuf[UartBufLimit]; 




#define NET_EVNET_START (0x00000051)// the max is 255

typedef enum
{
    eNETEventPairedOK=NET_EVNET_START,//81 update  max syn WIFI
	eNETEventExitPairedOK,
	//Alarm
	eNETEventSmokeAlarm,
	eNETEventCoAlarm,
	eNETEventCancleAlarm,//84
	eNETEventMute,
	eNETEventCancelMute,
	eNETEventSensorErr,


	eNETEventTest,
	eNETEventTestFinished,
	eNETEventOnline,
	eNETEventOutoffLine,//90
	eNETEventRemoteIR,
	eNETEventAck,
	eNETEventTimeout,
	eNETEventPairStart,
	eNETEventMax,
}eNETEvent;

typedef enum _ePairedMode
 {
	 ePairedNull = 0,
	 ePairedHub,
	 ePairedIntranet,
	 
 }ePairedMode;





typedef enum{
  NetDisconnect = 0x00,
  NetConnect,
  NetAlarm,
  NetAlarmSelf,
  NetMute,
  NetMuteSelf,
  NetTest,
}NetState;
  
typedef enum _NetMsgType{
 NetMsgNull = 0x00,
 NetMsgSmokeAlarm,
 NetMsgCancleAlarm,
 NetMsgMute,
 NetMsgCancleMute,
 NetMsgTest,//0x05
 NetMsgTestFinished,
 NetMsgPairRx,  
 RfMsgPairFactoryRx, 
 NetMsgCanclePair,
 NetMsgBurnID,  //0x0A
 NetMsgPairOK,  
 NetMsgOnLine,  
 NetMsgOffLine,  
 NetMsgAck,//0xE
 NetStopMasterAlarm,//0x0f for ir
 NetMsgCOAlarm, 
 NetMsgPairTx,
 NetMsgResetID,
}NetMsgType;




/***************************************************函数声明******************************************/
void *my_memset(void *src,unsigned char ch,unsigned short count);


void Net_data_handle(unsigned short offset);


void NetMain(void);
void NetInit(void);
void UartSendMsg(eUartMsgType UartMsgType, uint8_t MsgLevel1, uint8_t Data1, uint8_t Data2);
void UartSendReMsg(eUartMsgType UartMsgType, uint8_t MsgLevel1, uint8_t Data1, uint8_t Data2);



void NetTestMode(SystemApiTag  *me,QEvent *e);
void NetTxPairMode(SystemApiTag  *me,QEvent *e);
void NetExitPairMode(SystemApiTag  *me,QEvent *e);
void NetRxPairMode(SystemApiTag  *me,QEvent *e);
void NetSendTestMode(SystemApiTag  *me,QEvent *e);
void NETFactoryMode(SystemApiTag  *me,QEvent *e);
void NetPairingHubMode(SystemApiTag  *me,QEvent *e);






#endif/*endif for __SUPPORT_RF_FUNCTION__*/


#endif/*endif for _RF_H_*/

