#ifndef _WIFIUPDATA_H_
#define _WIFIUPDATA_H_
#include "base.h"


#define WiFiUpdataNew

#ifdef WiFiUpdataNew
typedef struct _Data_WiFi
{
	uint8_t DPID;
	uint16_t data;
}DATAWIFI;
#define  UpdataMax     3
extern DATAWIFI UpdataWifi[UpdataMax];
#endif



#define WIFI_POWER_PORT  GPIOC
#define WIFI_POWER_PIN  GPIO_Pin_4

#define WIFI_POWER_ON()  GPIO_SetBits(WIFI_POWER_PORT,WIFI_POWER_PIN)
#define WIFI_POWER_OFF() GPIO_ResetBits(WIFI_POWER_PORT,WIFI_POWER_PIN)

#define WiFiHalfONEDAY   (4320)	//S<255 song 2020 8 3



#define SUPPORT_UPGRADE_FIRMWARE
#define SUPPORT_UPGRADE_FIRMWARE_TIMEOUT   (180)	//S<255 song 2020 8 3


#define FACTRORY_CHECKWIFI_FLAG          (0x1A)

#define WIFIRECONNECTEDTIMES   (4)

#define NET_EVNET_START (0x00000051)// the max is 255
#define  RFLEDFLASHNUM		(250)

#define  RFLEDFLASHON		(50)

void WifiDeinit(void);
void WifiInit(void);
void CheckWiFi(SystemApiTag *me);

void WifiVarInit(void);
void WiFi_Reset(uint8_t config);
void WiFi_EN(uint8_t config);

void WIFIStateHandle(uint8_t WifiState);
void PollingFirmwareUpgade(uint16_t Time);

void NetConnetPolling(TmrHdl Handle, eTmrCmd eCmd);//����
void PollingWifiTimeout(TmrHdl Handle, eTmrCmd eCmd);//�ϴ��ɹ�ɾ��polling
void PollingWifiUpgradeTimeout(TmrHdl Handle, eTmrCmd eCmd);

void Get_DpId_Data(uint8_t dpid , uint8_t value);
void Smoke_all_data_update(void);
void SmokeMuteCancel(void);
void SmokeSetAllUpdateFlag(void);
void CalibrationOKIndex(void);
void CalibrationErrIndex(void);
void WiFiCheckOneDay(uint32_t time);
#ifdef WiFiUpdataNew
//void SetWiFiUpdata(uint8_t dpid,uint16_t Value);
void SetWiFiUpdata(uint8_t dpid,uint16_t Value,uint8_t dpid1,uint16_t Value1,uint8_t dpid2,uint16_t Value2);

void ReSetWiFiUpdata(void);
void WiFiOpenCheck(void);
void WIFIEZ(void);
void WIFIAP(void);
void WiFiRSSIDisPlay(void);

#endif

//void SmokeMuteCmd(void);

#endif
