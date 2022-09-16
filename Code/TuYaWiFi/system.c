/****************************************Copyright (c)*************************
**                               ��Ȩ���� (C), 2015-2020, Ϳѻ�Ƽ�
**
**                                 http://www.tuya.com
**
**--------------�ļ���Ϣ-------------------------------------------------------
**��   ��   ��: system.c
**��        ��: wifi���ݴ�������
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
#define SYSTEM_GLOBAL
#include "main.h"
#include "wifi.h"
#include "uart.h"
#include "smoke.h"
#include "co.h"

//
#ifdef SUPPORT_WIFI_NET_FUNCTION
extern const DOWNLOAD_CMD_S download_cmd[];
unsigned short firm_size;                                                      //������һ���Ĵ�С

/*****************************************************************************
�������� : set_wifi_uart_byte
�������� : дwifi_uart�ֽ�
������� : dest:��������ʵ��ַ;
           byte:д���ֽ�ֵ
���ز��� : д����ɺ���ܳ���
*****************************************************************************/
unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte)
{
  unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;
  
  *obj = byte;
  dest += 1;
  
  return dest;
}
/*****************************************************************************
�������� : set_wifi_uart_buffer
�������� : дwifi_uart_buffer
������� : dest:Ŀ���ַ
           src:Դ��ַ
           len:���ݳ���
���ز��� : ��
*****************************************************************************/
unsigned short set_wifi_uart_buffer(unsigned short dest, unsigned char *src, unsigned short len)
{
  unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;
  
  my_memcpy(obj,src,len);
  
  dest += len;
  return dest;
}
/*****************************************************************************
�������� : wifi_uart_write_data
�������� : ��wifi uartд����������
������� : in:���ͻ���ָ��
           len:���ݷ��ͳ���
���ز��� : ��
*****************************************************************************/
static void wifi_uart_write_data(unsigned char *in, unsigned short len)
{
  if((NULL == in) || (0 == len))
  {
    return;
  }
  
  while(len --)
  {
     UartSendData(*in);
     in ++;
  }
}
/*****************************************************************************
�������� : get_check_sum
�������� : ����У���
������� : pack:����Դָ��
           pack_len:����У��ͳ���
���ز��� : У���
*****************************************************************************/
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
  unsigned short i;
  unsigned char check_sum = 0;
  
  for(i = 0; i < pack_len; i ++)
  {
    check_sum += *pack ++;
  }
  
  return check_sum;
}
/*****************************************************************************
�������� : wifi_uart_write_frame
�������� : ��wifi���ڷ���һ֡����
������� : fr_type:֡����
           len:���ݳ���
���ز��� : ��
*****************************************************************************/
void wifi_uart_write_frame(unsigned char fr_type, unsigned short len)
{
  unsigned char check_sum = 0;
  
  wifi_uart_tx_buf[HEAD_FIRST] = 0x55;
  wifi_uart_tx_buf[HEAD_SECOND] = 0xaa;
  wifi_uart_tx_buf[PROTOCOL_VERSION] = 0x00;
  wifi_uart_tx_buf[FRAME_TYPE] = fr_type;//Updata Cmd 05// WiFi Work state 02
  wifi_uart_tx_buf[LENGTH_HIGH] = len >> 8;
  wifi_uart_tx_buf[LENGTH_LOW] = len & 0xff;
  #ifdef SUPPORT_NET_FUNCTION_DEBUG
  printf("len:%d\n",(int)len);
  #endif
  len += PROTOCOL_HEAD;
  check_sum = get_check_sum((unsigned char *)wifi_uart_tx_buf, len - 1);
  wifi_uart_tx_buf[len - 1] = check_sum;
  #ifdef SUPPORT_NET_FUNCTION_DEBUG
  printf("check_sum:0x%x\n",(int)check_sum);
  #endif
  wifi_uart_write_data((unsigned char *)wifi_uart_tx_buf, len);
}
/*****************************************************************************
�������� : heat_beat_check
�������� : ���������
������� : ��
���ز��� : ��
*****************************************************************************/
static void heat_beat_check(void)
{
  unsigned char length = 0;
  static unsigned char mcu_reset_state = FALSE;
  
  if(FALSE == mcu_reset_state)
  {
    length = set_wifi_uart_byte(length,FALSE);
    mcu_reset_state = TRUE;
  }
  else
  {
    length = set_wifi_uart_byte(length,TRUE);
  }
  
  wifi_uart_write_frame(HEAT_BEAT_CMD, length);
}

/*****************************************************************************
��������  : product_info_update
��������  : ��Ʒ��Ϣ�ϴ�
������� : ��
���ز��� : ��
*****************************************************************************/
static void product_info_update(void)
{
  unsigned char length = 0;
  
  length = set_wifi_uart_buffer(length,(unsigned char *)"{\"p\":\"",my_strlen((unsigned char *)"{\"p\":\""));
  length = set_wifi_uart_buffer(length,(unsigned char *)PRODUCT_KEY,my_strlen((unsigned char *)PRODUCT_KEY));
  length = set_wifi_uart_buffer(length,(unsigned char *)"\",\"v\":\"",my_strlen((unsigned char *)"\",\"v\":\""));
  length = set_wifi_uart_buffer(length,(unsigned char *)SoftWareVer,my_strlen((unsigned char *)SoftWareVer));
//  length = set_wifi_uart_buffer(length,(unsigned char *)MCU_VER,my_strlen((unsigned char *)MCU_VER));
  length = set_wifi_uart_buffer(length,(unsigned char *)"\"}",my_strlen((unsigned char *)"\"}"));
  
  wifi_uart_write_frame(PRODUCT_INFO_CMD, length);
}
/*****************************************************************************
�������� : get_update_dpid_index
�������� : �����ƶ�DPID�������е����
������� : dpid:dpid
���ز��� : index:dp���
*****************************************************************************/
static unsigned char get_dowmload_dpid_index(unsigned char dpid)
{
  unsigned char index;
  unsigned char total = get_download_cmd_total();
  
  for(index = 0; index < total; index ++)
  {
    if(download_cmd[index].dp_id == dpid)
    {
      break;
    }
  }
  
  return index;
}
/*****************************************************************************
�������� : data_point_handle
�������� : �·����ݴ���
������� : value:�·�����Դָ��
���ز��� : ret:�������ݴ������
*****************************************************************************/
static unsigned char data_point_handle(const unsigned char value[])
{
  unsigned char dp_id,index;
  unsigned char dp_type;
  unsigned char ret;
  unsigned short dp_len;
  
  dp_id = value[0];
  dp_type = value[1];
  dp_len = value[2] * 0x100;
  dp_len += value[3];
  
  index = get_dowmload_dpid_index(dp_id);

  if(dp_type != download_cmd[index].dp_type)
  {
    //������ʾ
    return FALSE;
  }
  else
  {
    ret = dp_download_handle(dp_id,value + 4,dp_len);
  }
  
  return ret;
}
/*****************************************************************************
�������� : data_handle
�������� : ����֡����
������� : offset:������ʼλ
���ز��� : ��
*****************************************************************************/
static QEvent WIFIEVENT;

void data_handle(unsigned short offset)
{
#ifdef SUPPORT_MCU_FIRM_UPDATE
      unsigned char *firmware_addr;
      static unsigned long firm_length;                                             //MCU�����ļ�����
      static unsigned char firm_update_flag;                                        //MCU������־
      unsigned long dp_len;
      unsigned char firm_flag;                                                      //��������С��־
#else
  unsigned short dp_len;
#endif
  unsigned char ret;
  unsigned short i,total_len;
  unsigned char cmd_type = wifi_uart_rx_buf[offset + FRAME_TYPE];
  
  switch(cmd_type)
  {
  case HEAT_BEAT_CMD:                                   //������
    heat_beat_check();
    break;
    
  case PRODUCT_INFO_CMD:                                //��Ʒ��Ϣ
    product_info_update();
	WIFIEVENT.sig =eSystemEventWifiPairStart;
	SystemPostFifo(&WIFIEVENT);
	#if 0
	if(SysAO.eMode == WifiFactoryTestMode)
	{
		
		SysteSetSig(eEventSigWifiRssiTestStart);
	}
	else
	{
		SysteSetSig(eEventSigWifiPairStart);
	}
	#endif
    break;
    
#ifndef WIFI_CONTROL_SELF_MODE
  case WIFI_STATE_CMD:                                  //wifi����״̬	
    wifi_work_state = wifi_uart_rx_buf[offset + DATA_START];	
	#ifdef SUPPORT_NET_FUNCTION_DEBUG
  	printf("wifistate:%d\n",(int)wifi_work_state);
	#endif
    wifi_uart_write_frame(WIFI_STATE_CMD,0);
    break;

#ifdef SUPPORT_UPGRADE_FIRMWARE
	case WIFI_UPDATE_CMD:
	wifi_firmwareupgrade_state = wifi_uart_rx_buf[offset + DATA_START];	
	#ifdef SUPPORT_NET_FUNCTION_DEBUG
  	printf("update:%d\n",(int)wifi_firmwareupgrade_state);
	#endif	
	break;
#endif
  case STATE_UPLOAD_CMD:								//state 	  
	#ifdef SUPPORT_NET_FUNCTION_DEBUG
  	printf("wifi ack:%d\n",(int)wifi_uart_rx_buf[offset + DATA_START]);
	#endif	
	if(wifi_uart_rx_buf[offset + DATA_START]==0)
	{
//		printf("GetSmokeState(%d)-GetCOState(%d) \n",GetSmokeState(),GetCOState());
		if((GetSmokeState()==eSmokeState_Alarm)||(GetCOState()==eCOed))
		{
			
		}
		else
		{
			WifiAO.WifiCMDTimeout = 4;
		}
	}
	
	break;

  case WIFI_RESET_CMD:                                  //����wifi(wifi���سɹ�)
    reset_wifi_flag = RESET_WIFI_SUCCESS;
	
    break;
    
  case WIFI_MODE_CMD:                                   //ѡ��smartconfig/APģʽ(wifi���سɹ�)	
    set_wifimode_flag = SET_WIFICONFIG_SUCCESS;
    break;
#endif
    
  case DATA_QUERT_CMD:                                  //�����·�
    total_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] * 0x100;
    total_len += wifi_uart_rx_buf[offset + LENGTH_LOW];
    
    for(i = 0;i < total_len;)
    {
      dp_len = wifi_uart_rx_buf[offset + DATA_START + i + 2] * 0x100;
      dp_len += wifi_uart_rx_buf[offset + DATA_START + i + 3];
      //
      ret = data_point_handle((unsigned char *)wifi_uart_rx_buf + offset + DATA_START + i);
      
      if(SUCCESS == ret)
      {
        //�ɹ���ʾ
      }
      else
      {
        //������ʾ
      }
      
      i += (dp_len + 4);
    }
    
    break;
    case ROUTE_RSSI_CMD:
		if(wifi_uart_rx_buf[offset + DATA_START]==0)
		{
			WifiAO.WifiRSSI=0;
		}else
		{
			WifiAO.WifiRSSI=wifi_uart_rx_buf[offset + DATA_START + 1];
		}
//		printf("RSSI: %d\n",WifiAO.WifiRSSI);
		WiFiRSSIDisPlay();
		break;
#ifdef SUPPORT_MCU_RTC_CHECK
  case GET_LOCAL_TIME_CMD:                             //��ȡ����ʱ��
      mcu_write_rtctime(wifi_uart_rx_buf + offset + DATA_START);
    break;
#endif
 
#ifdef WIFI_TEST_ENABLE
  case WIFI_TEST_CMD: 
    wifi_test_result(wifi_uart_rx_buf[offset + DATA_START],wifi_uart_rx_buf[offset + DATA_START + 1]);
    break;
#endif
    
  default:
    break;
  }
}
/*****************************************************************************
�������� : get_queue_total_data
�������� : ��ȡ����������
������� : ��
���ز��� : ��
*****************************************************************************/
unsigned char get_queue_total_data(void)
{
  if(queue_in != queue_out)
    return 1;
  else
    return 0;
}
/*****************************************************************************
�������� : Queue_Read_Byte
�������� : ��ȡ����1�ֽ�����
������� : ��
���ز��� : ��
*****************************************************************************/
unsigned char Queue_Read_Byte(void)
{
  unsigned char value = 0;
  
  if(queue_out != queue_in)
  {
    //������
    if(queue_out >= (unsigned char *)(wifi_queue_buf + sizeof(wifi_queue_buf)))
    {
      //�����Ѿ���ĩβ
      queue_out = (unsigned char *)(wifi_queue_buf);
    }
    
    value = *queue_out ++;   
  }
  
  return value;
}

#endif