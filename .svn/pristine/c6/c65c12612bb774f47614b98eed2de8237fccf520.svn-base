/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: wifi.h
**描        述: wifi文件头定义
**使 用 说 明 : 用户无需关心该文件实现内容
**
**
**--------------当前版本修订---------------------------------------------------
** 版  本: v1.0.2
** 日　期: 2019年8月14日
** 描　述: 1 增大发送缓存大小为最小48
           2 增加请求WIFI模块固件升级功能
           3 增加查询当前连接路由器信号强度
           4 增加mcu升级操作（ 请求升级 升级开始 升级传输 ）
           5 增加获取dp缓存指令

** 版  本: v1.0.1
** 日　期: 2018年11月1日
** 描　述: 1:低功耗SDK上线正式版

** 版  本: v1.0
** 日　期: 2018年4月24日
** 描　述: 1:低功耗SDK首版，适配1.0.2协议
**
**-----------------------------------------------------------------------------
******************************************************************************/
#ifndef __WIFI_H_
#define __WIFI_H_
#include "WiFiUpdata.h"

#ifndef INVALID
#define         INVALID             0xFF
#endif

//=============================================================================
//dp数据点类型
//=============================================================================
#define         DP_TYPE_RAW                     0x00				//RAW型
#define         DP_TYPE_BOOL                    0x01	            //布尔型
#define         DP_TYPE_VALUE                   0x02	            //数值型
#define         DP_TYPE_STRING                  0x03				//字符串型
#define         DP_TYPE_ENUM                    0x04				//枚举型
#define         DP_TYPE_FAULT                   0x05				//故障型

//=============================================================================
//WIFI工作状态
//=============================================================================
#define         SMART_CONFIG_STATE              0x00
#define         AP_STATE                        0x01
#define         WIFI_NOT_CONNECTED              0x02
#define         WIFI_CONNECTED                  0x03
#define			WIFI_CONN_CLOUD					0x04
#define         WIFI_SATE_UNKNOW                0xff
//=============================================================================
//wifi配网的方式
//=============================================================================
#define         SMART_CONFIG                    0x0  
#define         AP_CONFIG                       0x1   

//=============================================================================
//wifi复位状态
//=============================================================================
#define         RESET_WIFI_ERROR                0
#define         RESET_WIFI_SUCCESS              1

//=============================================================================
//wifi复位状态
//=============================================================================
#define         SET_WIFICONFIG_ERROR            0
#define         SET_WIFICONFIG_SUCCESS          1

#ifdef SUPPORT_UPGRADE_FIRMWARE
//=============================================================================
//WIFI固件升级状态
//=============================================================================
#define         WIFI_FIRM_START_CHECKING              0x00                      //开始检测固件更新
#define         WIFI_FIRM_IS_LAST                     0x01                      //固件已经是最新
#define         WIFI_FIRM_IS_UPGRADING                0x02                      //固件更新中
#define         WIFI_FIRM_UPGRADE_SUCCESS             0x03                      //固件更新成功
#define         WIFI_FIRM_UPGRADE_FAIL                0x04                      //固件更新失败
#endif

//=============================================================================
//MCU固件升级状态
//=============================================================================
#define         FIRM_STATE_UN_SUPPORT           0x00                            //不支持 MCU 升级
#define         FIRM_STATE_WIFI_UN_READY        0x01                            //模块未就绪
#define         FIRM_STATE_GET_ERROR            0x02                            //云端升级信息查询失败
#define         FIRM_STATE_NO                   0x03                            //无需升级（云端无更新版本）
#define         FIRM_STATE_START                0x04                            //需升级，等待模块发起升级操作

//=============================================================================
//WIFI和mcu的工作方式 
//=============================================================================
#define         UNION_WORK                      0x0                             //mcu模块与wifi配合处理
#define         WIFI_ALONE                      0x1                             //wifi模块自处理

//=============================================================================
//系统工作模式
//=============================================================================
#define         NORMAL_MODE             0x00                                    //正常工作状态
#define         FACTORY_MODE            0x01                                    //工厂模式	
#define         UPDATE_MODE             0X02                                    //升级模式	     

#include "protocol.h"
#include "system.h"
#include "mcu_api.h"
#include "uart.h"
#include "WiFiUpdata.h"
//=============================================================================
//下发命令
//=============================================================================
typedef struct {
  unsigned char dp_id;                        //dp序号
  unsigned char dp_type;                          //dp类型
} DOWNLOAD_CMD_S;

typedef enum{	
	eSTAMode = 0,
	eAPMode,	
	eMAXMode,	
}eResetMode;
	
typedef enum{	
	eWIFIDisConnected = 0,
	eWIFIConnecting,	
	eWIFIConnected,	
	eWIFIClosing,
	eWIFIClosed,
}eWIFIState;
typedef struct WifiObject{
	uint8_t WifiUpdateState;// wifi update state
	uint8_t WifiUpdateValue;//wifi update  value
	bool WifiCONNFlag;  // wifi can update flag
	bool WifiCheckUpdateFlag;// wifi check model upgrade
	bool WifiUpdateFlag;// wifi is upgrading
	bool WifiResetFlag; // wifi reset sta or ap
	bool Wifiheartbeat; // 
	eResetMode eUpgradeFlag;//which state 
	uint8_t UpgradeTimeoutCnt;//
	uint8_t WifiCMDTimeout; 
	uint8_t WifiFactory;
	bool WifiPiredFlag; //pired or not
	uint8_t WifiReconnectNum;//reconnect num
	uint8_t FirstOpenFlag;
	uint8_t WifiRSSI; 
	
	uint8_t PirState;

}WifiObjectTag;
extern  WifiObjectTag WifiAO;

#endif
