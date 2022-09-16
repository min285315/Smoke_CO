/****************************************Copyright (c)*************************
**                               ��Ȩ���� (C), 2015-2020, Ϳѻ�Ƽ�
**
**                                 http://www.tuya.com
**
**--------------�ļ���Ϣ-------------------------------------------------------
**��   ��   ��: wifi.h
**��        ��: wifi�ļ�ͷ����
**ʹ �� ˵ �� : �û�������ĸ��ļ�ʵ������
**
**
**--------------��ǰ�汾�޶�---------------------------------------------------
** ��  ��: v1.0.2
** �ա���: 2019��8��14��
** �衡��: 1 �����ͻ����СΪ��С48
           2 ��������WIFIģ��̼���������
           3 ���Ӳ�ѯ��ǰ����·�����ź�ǿ��
           4 ����mcu���������� �������� ������ʼ �������� ��
           5 ���ӻ�ȡdp����ָ��

** ��  ��: v1.0.1
** �ա���: 2018��11��1��
** �衡��: 1:�͹���SDK������ʽ��

** ��  ��: v1.0
** �ա���: 2018��4��24��
** �衡��: 1:�͹���SDK�װ棬����1.0.2Э��
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
//dp���ݵ�����
//=============================================================================
#define         DP_TYPE_RAW                     0x00				//RAW��
#define         DP_TYPE_BOOL                    0x01	            //������
#define         DP_TYPE_VALUE                   0x02	            //��ֵ��
#define         DP_TYPE_STRING                  0x03				//�ַ�����
#define         DP_TYPE_ENUM                    0x04				//ö����
#define         DP_TYPE_FAULT                   0x05				//������

//=============================================================================
//WIFI����״̬
//=============================================================================
#define         SMART_CONFIG_STATE              0x00
#define         AP_STATE                        0x01
#define         WIFI_NOT_CONNECTED              0x02
#define         WIFI_CONNECTED                  0x03
#define			WIFI_CONN_CLOUD					0x04
#define         WIFI_SATE_UNKNOW                0xff
//=============================================================================
//wifi�����ķ�ʽ
//=============================================================================
#define         SMART_CONFIG                    0x0  
#define         AP_CONFIG                       0x1   

//=============================================================================
//wifi��λ״̬
//=============================================================================
#define         RESET_WIFI_ERROR                0
#define         RESET_WIFI_SUCCESS              1

//=============================================================================
//wifi��λ״̬
//=============================================================================
#define         SET_WIFICONFIG_ERROR            0
#define         SET_WIFICONFIG_SUCCESS          1

#ifdef SUPPORT_UPGRADE_FIRMWARE
//=============================================================================
//WIFI�̼�����״̬
//=============================================================================
#define         WIFI_FIRM_START_CHECKING              0x00                      //��ʼ���̼�����
#define         WIFI_FIRM_IS_LAST                     0x01                      //�̼��Ѿ�������
#define         WIFI_FIRM_IS_UPGRADING                0x02                      //�̼�������
#define         WIFI_FIRM_UPGRADE_SUCCESS             0x03                      //�̼����³ɹ�
#define         WIFI_FIRM_UPGRADE_FAIL                0x04                      //�̼�����ʧ��
#endif

//=============================================================================
//MCU�̼�����״̬
//=============================================================================
#define         FIRM_STATE_UN_SUPPORT           0x00                            //��֧�� MCU ����
#define         FIRM_STATE_WIFI_UN_READY        0x01                            //ģ��δ����
#define         FIRM_STATE_GET_ERROR            0x02                            //�ƶ�������Ϣ��ѯʧ��
#define         FIRM_STATE_NO                   0x03                            //�����������ƶ��޸��°汾��
#define         FIRM_STATE_START                0x04                            //���������ȴ�ģ�鷢����������

//=============================================================================
//WIFI��mcu�Ĺ�����ʽ 
//=============================================================================
#define         UNION_WORK                      0x0                             //mcuģ����wifi��ϴ���
#define         WIFI_ALONE                      0x1                             //wifiģ���Դ���

//=============================================================================
//ϵͳ����ģʽ
//=============================================================================
#define         NORMAL_MODE             0x00                                    //��������״̬
#define         FACTORY_MODE            0x01                                    //����ģʽ	
#define         UPDATE_MODE             0X02                                    //����ģʽ	     

#include "protocol.h"
#include "system.h"
#include "mcu_api.h"
#include "uart.h"
#include "WiFiUpdata.h"
//=============================================================================
//�·�����
//=============================================================================
typedef struct {
  unsigned char dp_id;                        //dp���
  unsigned char dp_type;                          //dp����
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
