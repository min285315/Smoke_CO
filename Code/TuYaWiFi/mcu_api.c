/****************************************Copyright (c)*************************
**                               ��Ȩ���� (C), 2015-2020, Ϳѻ�Ƽ�
**
**                                 http://www.tuya.com
**
**--------------�ļ���Ϣ-------------------------------------------------------
**��   ��   ��: mcu_api.c
**��        ��: �·�/�ϱ����ݴ�����
**ʹ �� ˵ �� : ���ļ��º��������û��޸�,�û���Ҫ���õ��ļ����ڸ��ļ���
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
#define MCU_API_GLOBAL
#include "sys_cfg.h"
#include "wifi.h"
#include "system.h"
#include "WIFIUpdata.h"
#include "Module.h"
#include "SceneInstance.h"
#include "main.h"

#ifdef SUPPORT_WIFI_NET_FUNCTION
/*****************************************************************************
�������� : hex_to_bcd
�������� : hexתbcd
������� : Value_H:���ֽ�/Value_L:���ֽ�
���ز��� : bcd_value:ת����ɺ�����
*****************************************************************************/
#if 0
unsigned char hex_to_bcd(unsigned char Value_H,unsigned char Value_L)
{
  unsigned char bcd_value;
  
  if((Value_H >= '0') && (Value_H <= '9'))
    Value_H -= '0';
  else if((Value_H >= 'A') && (Value_H <= 'F'))
    Value_H = Value_H - 'A' + 10;
  else if((Value_H >= 'a') && (Value_H <= 'f'))
    Value_H = Value_H - 'a' + 10;
   
  bcd_value = Value_H & 0x0f;
  
  bcd_value <<= 4;
  if((Value_L >= '0') && (Value_L <= '9'))
    Value_L -= '0';
  else if((Value_L >= 'A') && (Value_L <= 'F'))
    Value_L = Value_L - 'a' + 10;
  else if((Value_L >= 'a') && (Value_L <= 'f'))
    Value_L = Value_L - 'a' + 10;
  
  bcd_value |= Value_L & 0x0f;

  return bcd_value;
}
#endif
/*****************************************************************************
�������� : my_strlen
�������� : ���ַ�������
������� : src:Դ��ַ
���ز��� : len:���ݳ���
*****************************************************************************/
unsigned long my_strlen(unsigned char *str)  
{
  unsigned long len = 0;
  if(str == NULL)
  { 
    return 0;
  }
  
  for(len = 0; *str ++ != '\0'; )
  {
    len ++;
  }
  
  return len;
}
/*****************************************************************************
�������� : my_memset
�������� : ��src��ָ�ڴ������ǰcount���ֽ����ó��ַ�c
������� : src:Դ��ַ
           ch:�����ַ�
           count:�������ݳ���
���ز��� : src:���ݴ�������Դ��ַ
*****************************************************************************/
#if 0
void *my_memset(void *src,unsigned char ch,unsigned short count)
{
  unsigned char *tmp = (unsigned char *)src;
  
  if(src == NULL)
  {
    return NULL;
  }
  
  while(count --)
  {
    *tmp ++ = ch;
  }
  
  return src;
}
#endif
/*****************************************************************************
�������� : mymemcpy
�������� : �ڴ濽��
������� : dest:Ŀ���ַ
           src:Դ��ַ
           count:���ݿ�������
���ز��� : src:���ݴ�������Դ��ַ
*****************************************************************************/
void *my_memcpy(void *dest, const void *src, unsigned short count)  
{  
  unsigned char *pdest = (unsigned char *)dest;  
  const unsigned char *psrc  = (const unsigned char *)src;  
  unsigned short i;
  
  if(dest == NULL || src == NULL)
  { 
    return NULL;
  }
  
  if((pdest <= psrc) || (pdest > psrc + count))
  {  
    for(i = 0; i < count; i ++)
    {  
      pdest[i] = psrc[i];  
    }  
  }
  else
  {
    for(i = count; i > 0; i --)
    {  
      pdest[i - 1] = psrc[i - 1];  
    }  
  }  
  
  return dest;  
}
/*****************************************************************************
�������� : my_strcpy
�������� : �ڴ濽��
������� : s1:Ŀ���ַ
           s2:Դ��ַ
���ز��� : ���ݴ�������Դ��ַ
*****************************************************************************/
#if 0
char *my_strcpy(char *dest, const char *src)  
{
  char *p = dest;
  if(dest == NULL || src == NULL)
  { 
    return NULL;
  }
  while(*src!='\0')
  {
    *dest++ = *src++;
  }
  *dest = '\0';
  return p;
}
#endif
/*****************************************************************************
�������� : my_strcmp
�������� : �ڴ濽��
������� : s1:�ַ���1
           s2:�ַ���2
���ز��� : ��С�Ƚ�ֵ��0:s1=s2; -1:s1<s2; 1:s1>s2
*****************************************************************************/
#if 0
int my_strcmp(char *s1 , char *s2)
{
  if(s1 == NULL || s2 == NULL)
  { 
	return 0;
  }

  while( *s1 && *s2 && *s1 == *s2 )
  {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}
#endif
/*****************************************************************************
�������� : int_to_byte
�������� : ��int���Ͳ���ĸ��ֽ�
������� : number:4�ֽ�ԭ����;value:������ɺ�4�ֽ�����
���ز��� :��
****************************************************************************/
#if 0
void int_to_byte(unsigned long number,unsigned char value[4])
{
  value[0] = number >> 24;
  value[1] = number >> 16;
  value[2] = number >> 8;
  value[3] = number & 0xff;
}
#endif
/*****************************************************************************
�������� : byte_to_int
�������� : ��4�ֽںϲ�Ϊ1��32bit����
������� : value:4�ֽ�����
���ز��� : number:�ϲ���ɺ��32bit����
****************************************************************************/
#if 0
unsigned long byte_to_int(const unsigned char value[4])
{
  unsigned long nubmer = 0;

  nubmer = (unsigned long)value[0];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[1];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[2];
  nubmer <<= 8;
  nubmer |= (unsigned long)value[3];
  
  return nubmer;
}
#endif
#ifndef WIFI_CONTROL_SELF_MODE
/*****************************************************************************
�������� : mcu_get_reset_wifi_flag
�������� : MCU��ȡ��λwifi�ɹ���־
������� : ��
���ز��� : ��λ��־:RESET_WIFI_ERROR:ʧ��/RESET_WIFI_SUCCESS:�ɹ�
ʹ��˵�� : 1:MCU��������mcu_reset_wifi()����øú�����ȡ��λ״̬
           2:���Ϊģ���Դ���ģʽ,MCU������øú���
*****************************************************************************/
#if 0
unsigned char mcu_get_reset_wifi_flag(void)
{
  return reset_wifi_flag;
}
#endif
/*****************************************************************************
�������� : reset_wifi
�������� : MCU��������wifi����ģʽ
������� : ��
���ز��� : ��
ʹ��˵�� : 1:MCU��������,ͨ��mcu_get_reset_wifi_flag()������ȡ����wifi�Ƿ�ɹ�
           2:���Ϊģ���Դ���ģʽ,MCU������øú���
*****************************************************************************/
#if 0
void mcu_reset_wifi(void)
{
  reset_wifi_flag = RESET_WIFI_ERROR;
  
  wifi_uart_write_frame(WIFI_RESET_CMD, 0);
}
#endif
/*****************************************************************************
�������� : mcu_get_wifimode_flag
�������� : ��ȡ����wifi״̬�ɹ���־
������� : ��
���ز��� : SET_WIFICONFIG_ERROR:ʧ��/SET_WIFICONFIG_SUCCESS:�ɹ�
ʹ��˵�� : 1:MCU��������mcu_set_wifi_mode()����øú�����ȡ��λ״̬
           2:���Ϊģ���Դ���ģʽ,MCU������øú���
*****************************************************************************/
#if 0
unsigned char mcu_get_wifimode_flag(void)
{
  return set_wifimode_flag;
}
#endif

/*****************************************************************************
�������� : mcu_set_wifi_mode
�������� : MCU����wifi����ģʽ
������� : mode:
          SMART_CONFIG:����smartconfigģʽ
          AP_CONFIG:����APģʽ
���ز��� : ��
ʹ��˵�� : 1:MCU��������
           2:�ɹ���,���ж�set_wifi_config_state�Ƿ�ΪTRUE;TRUE��ʾΪ����wifi����ģʽ�ɹ�
           3:���Ϊģ���Դ���ģʽ,MCU������øú���
*****************************************************************************/
void mcu_set_wifi_mode(unsigned char mode)
{
  unsigned char length = 0;
  
  set_wifimode_flag = SET_WIFICONFIG_ERROR;
  wifi_work_state = WIFI_SATE_UNKNOW;
  SmokeSetAllUpdateFlag();

  length = set_wifi_uart_byte(length, mode);
  
  wifi_uart_write_frame(WIFI_MODE_CMD, length);
}
/*****************************************************************************
�������� : mcu_get_wifi_work_state
�������� : MCU������ȡ��ǰwifi����״̬
������� : ��
���ز��� : WIFI_WORK_SATE_E:
          SMART_CONFIG_STATE:smartconfig����״̬
          AP_STATE:AP ����״̬
          WIFI_NOT_CONNECTED:WIFI ���óɹ���δ����·����
          WIFI_CONNECTED:WIFI ���óɹ�������·����
          WIFI_CONN_CLOUD:WIFI �Ѿ��������Ʒ�����
ʹ��˵�� : ��
*****************************************************************************/
unsigned char mcu_get_wifi_work_state(void)
{
  return wifi_work_state;
}
#endif

#ifdef SUPPORT_MCU_RTC_CHECK
/*****************************************************************************
��������  : mcu_get_system_time
�������� : MCU��ȡϵͳʱ��,����У�Ա���ʱ��
������� : ��
���ز��� : ��
ʹ��˵�� : MCU����������ɺ���mcu_write_rtctime������У��rtcʱ��
*****************************************************************************/
void mcu_get_system_time(void)
{
  wifi_uart_write_frame(GET_LOCAL_TIME_CMD,0);
}
#endif

#ifdef WIFI_TEST_ENABLE
/*****************************************************************************
�������� : mcu_start_wifitest
�������� : mcu����wifi���ܲ���
������� : ��
���ز��� : ��
ʹ��˵�� : MCU��Ҫ���е��øù���
*****************************************************************************/
void mcu_start_wifitest(void)
{
  wifi_uart_write_frame(WIFI_TEST_CMD,0);
}
#endif

#ifdef SUPPORT_UPGRADE_FIRMWARE
void firmware_upgrade_check(void)
{
	wifi_uart_write_frame(WIFI_UPDATE_CMD, 0x00);		
}

unsigned char mcu_get_wifi_upgrade_state(void)
{
  return wifi_firmwareupgrade_state;
}
#endif

/*****************************************************************************
�������� : mcu_dp_raw_update
�������� : raw��dp�����ϴ�
������� : dpid:id��
           value:��ǰdpֵָ��
           len:���ݳ���
���ز��� : ��
*****************************************************************************/
#if 0
unsigned char mcu_dp_raw_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  //
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_RAW);
  //
  length = set_wifi_uart_byte(length,len / 0x100);
  length = set_wifi_uart_byte(length,len % 0x100);
  //
  length = set_wifi_uart_buffer(length,(unsigned char *)value,len);
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
#endif
/*****************************************************************************
�������� : mcu_dp_bool_update
�������� : bool��dp�����ϴ�
������� : dpid:id��
           value:��ǰdpֵ
���ز��� : ��
*****************************************************************************/
unsigned char mcu_dp_bool_update(unsigned char dpid,unsigned char value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_BOOL);
  //
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,1);
  //
  if(value == FALSE)
  {
    length = set_wifi_uart_byte(length,FALSE);
  }
  else
  {
    length = set_wifi_uart_byte(length,1);
  }
  
  if(value == FALSE)
  {
	length = set_wifi_uart_byte(length,DPID_CO_STATE);
	length = set_wifi_uart_byte(length,DP_TYPE_ENUM);
	//
	length = set_wifi_uart_byte(length,0);
	length = set_wifi_uart_byte(length,1);
	 //
	length = set_wifi_uart_byte(length,value);
  }
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
/*****************************************************************************
�������� : mcu_dp_value_update
�������� : value��dp�����ϴ�
������� : dpid:id��
           value:��ǰdpֵ
���ز��� : ��
*****************************************************************************/

unsigned char mcu_dp_value_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_VALUE);
  //
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,4);
  //
  length = set_wifi_uart_byte(length,value >> 24);
  length = set_wifi_uart_byte(length,value >> 16);
  length = set_wifi_uart_byte(length,value >> 8);
  length = set_wifi_uart_byte(length,value & 0xff);
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}

/*****************************************************************************
�������� : mcu_dp_string_update
�������� : rstring��dp�����ϴ�
������� : dpid:id��
           value:��ǰdpֵָ��
           len:���ݳ���
���ز��� : ��
*****************************************************************************/
#if 0
unsigned char mcu_dp_string_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  //
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_STRING);
  //
  length = set_wifi_uart_byte(length,len / 0x100);
  length = set_wifi_uart_byte(length,len % 0x100);
  //
  length = set_wifi_uart_buffer(length,(unsigned char *)value,len);
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}
#endif
/*****************************************************************************
�������� : mcu_dp_enum_update
�������� : enum��dp�����ϴ�
������� : dpid:id��
           value:��ǰdpֵ
���ز��� : ��
*****************************************************************************/
unsigned char mcu_dp_enum_update(unsigned char dpid,unsigned char value)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_ENUM);
  //
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,1);
  //
  length = set_wifi_uart_byte(length,value);
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
  
  return SUCCESS;
}

unsigned char mcu_dp_enum_update_co(unsigned char dpid,unsigned char value,uint32_t PPM)
{

	unsigned short length = 0;

  if(stop_update_flag == ENABLE)
  	return SUCCESS;

	length = set_wifi_uart_byte(length,dpid);
	length = set_wifi_uart_byte(length,DP_TYPE_ENUM);
  //
	length = set_wifi_uart_byte(length,0);
	length = set_wifi_uart_byte(length,1);
  //
	length = set_wifi_uart_byte(length,value);
// 
	length = set_wifi_uart_byte(length,DPID_CO_VALUE);
	length = set_wifi_uart_byte(length,DP_TYPE_VALUE);
//
	length = set_wifi_uart_byte(length,0);
	length = set_wifi_uart_byte(length,4);
//
	length = set_wifi_uart_byte(length,(uint8_t)(PPM >> 24));
	length = set_wifi_uart_byte(length,(uint8_t)(PPM >> 16));
	length = set_wifi_uart_byte(length,(uint8_t)(PPM >> 8));
	length = set_wifi_uart_byte(length,(uint8_t)(PPM & 0xff)); 
	wifi_uart_write_frame(STATE_UPLOAD_CMD,length); 
  return SUCCESS;
}

/*****************************************************************************
�������� : mcu_dp_fault_update
�������� : fault��dp�����ϴ�
������� : dpid:id��
           value:��ǰdpֵ
���ز��� : ��
*****************************************************************************/
unsigned char mcu_dp_fault_update(unsigned char dpid,unsigned long value)
{
  unsigned short length = 0;
   
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  length = set_wifi_uart_byte(length,dpid);
  length = set_wifi_uart_byte(length,DP_TYPE_FAULT);
  //
  length = set_wifi_uart_byte(length,0);
  
  if((value | 0xff) == 0xff)
  {
    length = set_wifi_uart_byte(length,1);
    length = set_wifi_uart_byte(length,value);
  }
  else if((value | 0xffff) == 0xffff)
  {
    length = set_wifi_uart_byte(length,2);
    length = set_wifi_uart_byte(length,value >> 8);
    length = set_wifi_uart_byte(length,value & 0xff);
  }
  else
  {
    length = set_wifi_uart_byte(length,4);
    length = set_wifi_uart_byte(length,value >> 24);
    length = set_wifi_uart_byte(length,value >> 16);
    length = set_wifi_uart_byte(length,value >> 8);
    length = set_wifi_uart_byte(length,value & 0xff);
  }    
  
  wifi_uart_write_frame(STATE_UPLOAD_CMD,length);

  return SUCCESS;
}
/*****************************************************************************
�������� : mcu_get_dp_download_bool
�������� : mcu��ȡbool���·�dpֵ
������� : value:dp���ݻ�������ַ
           length:dp���ݳ���
���ز��� : bool:��ǰdpֵ
*****************************************************************************/
#if 0
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len)
{
	static unsigned short Temp;
	Temp=len;
  return(value[0]);
}
#endif
/*****************************************************************************
�������� : mcu_get_dp_download_enum
�������� : mcu��ȡenum���·�dpֵ
������� : value:dp���ݻ�������ַ
           length:dp���ݳ���
���ز��� : enum:��ǰdpֵ
*****************************************************************************/
#if 0
unsigned char mcu_get_dp_download_enum(const unsigned char value[],unsigned short len)
{
  return(value[0]);
}
#endif
/*****************************************************************************
�������� : mcu_get_dp_download_value
�������� : mcu��ȡvalue���·�dpֵ
������� : value:dp���ݻ�������ַ
           length:dp���ݳ���
���ز��� : value:��ǰdpֵ
*****************************************************************************/
#if 0
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len)
{
  return(byte_to_int(value));
}
#endif

/*****************************************************************************
��������  : wifi_uart_service
��������  : wifi���ڴ������
������� : ��
���ز��� : ��
ʹ��˵�� : ��MCU������whileѭ���е��øú���
*****************************************************************************/
void wifi_uart_service(void)
{
  //#error "��ֱ����main������while(1){}�����wifi_uart_service(),���øú�����Ҫ���κ������ж�,��ɺ�ɾ������"
  //receive buf  data num
  static unsigned short rx_in = 0;
  unsigned short offset = 0;
  unsigned short rx_value_len = 0;             //����֡����
  //get data from intr rx  buf  to  rx receive buf
  while((rx_in < sizeof(wifi_uart_rx_buf)) && get_queue_total_data() > 0)
  {
	  wifi_uart_rx_buf[rx_in ++] = Queue_Read_Byte();
//		printf_log("wifi [%d] %x\n",rx_in,(int)wifi_uart_rx_buf[rx_in-1]);
  }
  // get data over  protocol head
  if(rx_in < PROTOCOL_HEAD)
    return;
  
  //check real  data 
  while((rx_in - offset) >= PROTOCOL_HEAD)
  {
  	// get frame header  0x55
    if(wifi_uart_rx_buf[offset + HEAD_FIRST] != FRAME_FIRST)
    {
      offset ++;  
//	  printf_log("offset0:%d\n",(int)offset);
      continue;
    }
    // get frame header  0xaa
    if(wifi_uart_rx_buf[offset + HEAD_SECOND] != FRAME_SECOND)
    {
      offset ++;
//	  printf_log("offset1:%d\n",(int)offset);
      continue;
    }  
    // get frame header  version
    if(wifi_uart_rx_buf[offset + PROTOCOL_VERSION] != VERSION)
    {
      offset += 2;
//	   printf_log("offset2:%d\n",(int)offset);
      continue;
    }      
    //get data packet length
    rx_value_len = wifi_uart_rx_buf[offset + LENGTH_HIGH] * 0x100 + wifi_uart_rx_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD;
    //data  packet lenght over  receive buf
	if(rx_value_len > sizeof(wifi_uart_rx_buf) + PROTOCOL_HEAD)
    {
      offset += 3;//give up err data :header frame  version LENGTH_HIGH LENGTH_LOW AND  DATA_START
 //      printf_log("offset3:%d\n",(int)offset);
      continue;
    }
    //data not receive over
    if((rx_in - offset) < rx_value_len)
    {
      break;
    }
    
    //���ݽ������,and compare check sum 
    if(get_check_sum((unsigned char *)wifi_uart_rx_buf + offset,rx_value_len - 1) != wifi_uart_rx_buf[offset + rx_value_len - 1])
    {
      //У�����
      offset += 3;//give up err data  header frame and version
//      printf_log("CheckSum err\n");
      continue;
    }
    //offset is right start position
    data_handle(offset);
    offset += rx_value_len;
  }//end while

  rx_in -= offset;
  //copy remain data to first position
  if(rx_in > 0)
  {
    my_memcpy(wifi_uart_rx_buf,wifi_uart_rx_buf + offset,rx_in);
  }
}
/*****************************************************************************
�������� :  wifi_protocol_init
�������� : Э�鴮�ڳ�ʼ������
������� : ��
���ز��� : ��
ʹ��˵�� : ������MCU��ʼ�������е��øú���
*****************************************************************************/
void wifi_protocol_init(void)
{
  //#error " ����main���������wifi_protocol_init()���wifiЭ���ʼ��,��ɾ������"
  queue_in = (unsigned char *)wifi_queue_buf;
  queue_out = (unsigned char *)wifi_queue_buf;
  //
  #ifndef WIFI_CONTROL_SELF_MODE
  wifi_work_state = WIFI_SATE_UNKNOW;
  #endif
}

#ifdef WiFiUpdataNew

static unsigned short mcudpboolupdate(unsigned char dpid,unsigned char value,unsigned short length)//消音
{
	length = set_wifi_uart_byte(length,dpid);
	length = set_wifi_uart_byte(length,DP_TYPE_BOOL);
	//
	length = set_wifi_uart_byte(length,0);
	length = set_wifi_uart_byte(length,1);
	//
	if(value == FALSE)
	{
		length = set_wifi_uart_byte(length,FALSE);
	}
	else
	{
		length = set_wifi_uart_byte(length,1);
	}
	return length;
}

static unsigned short mcudpvalueupdate(unsigned char dpid,unsigned long value,unsigned short length)
{
	length = set_wifi_uart_byte(length,dpid);
	length = set_wifi_uart_byte(length,DP_TYPE_VALUE);
	//
	length = set_wifi_uart_byte(length,0);
	length = set_wifi_uart_byte(length,4);
	//
	length = set_wifi_uart_byte(length,value >> 24);
	length = set_wifi_uart_byte(length,value >> 16);
	length = set_wifi_uart_byte(length,value >> 8);
	length = set_wifi_uart_byte(length,value & 0xff);

	return length;
}

static unsigned short mcudpenumupdate(unsigned char dpid,unsigned char value,unsigned short length)
{  
	length = set_wifi_uart_byte(length,dpid);
	length = set_wifi_uart_byte(length,DP_TYPE_ENUM);
	//
	length = set_wifi_uart_byte(length,0);
	length = set_wifi_uart_byte(length,1);
	//
	length = set_wifi_uart_byte(length,value);

	return length;
}

static unsigned short mcudpfaultupdate(unsigned char dpid,unsigned long value,unsigned short length)
{  
	length = set_wifi_uart_byte(length,dpid);
	length = set_wifi_uart_byte(length,DP_TYPE_FAULT);
	//
	length = set_wifi_uart_byte(length,0);
	
	if((value | 0xff) == 0xff)
	{
		length = set_wifi_uart_byte(length,1);
		length = set_wifi_uart_byte(length,value);
	}
	else if((value | 0xffff) == 0xffff)
	{
		length = set_wifi_uart_byte(length,2);
		length = set_wifi_uart_byte(length,value >> 8);
		length = set_wifi_uart_byte(length,value & 0xff);
	}
	else
	{
		length = set_wifi_uart_byte(length,4);
		length = set_wifi_uart_byte(length,value >> 24);
		length = set_wifi_uart_byte(length,value >> 16);
		length = set_wifi_uart_byte(length,value >> 8);
		length = set_wifi_uart_byte(length,value & 0xff);
	}
	return length;
}

unsigned char Mcu_dp_Update_All(void)
{
	unsigned short length = 0;
	uint8_t i;

	for(i=0;i<UpdataMax;i++)
	{
		//printf("dpid[%d]:%d\n",i,UpdataWifi[i].DPID);
		switch(UpdataWifi[i].DPID)
		{
			
			case DPID_CHECKING_RESULT:
			case DPID_SMOKE_SENSOR_STATE:
			case DPID_BATTERY_STATE:
			case DPID_CO_STATE:
				length=mcudpenumupdate(UpdataWifi[i].DPID,UpdataWifi[i].data,length);
			break;
			case DPID_FAULT:
				length=mcudpfaultupdate(UpdataWifi[i].DPID,UpdataWifi[i].data,length);
			break;
			case DPID_MUFFLING:
				length=mcudpboolupdate(UpdataWifi[i].DPID,UpdataWifi[i].data,length);
			break;
			case DPID_CO_VALUE:
				length=mcudpvalueupdate(UpdataWifi[i].DPID,(uint32_t)UpdataWifi[i].data,length);
			break;
		}
	}
	wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
	SetSysSleepFlag(eActiveFlagWifiTimeout);
	TmrInsertTimer(GetTmrHd(),TMR_PERIOD(1000),PollingWifiTimeout,(void*)(WifiAO.WifiUpdateState));
	return SUCCESS;
}

void APPInit(void)
{
	unsigned short length = 0;
	WifiAO.FirstOpenFlag=0;
	length=mcudpenumupdate(DPID_CHECKING_RESULT,SelfTestOK,length);
	length=mcudpenumupdate(DPID_SMOKE_SENSOR_STATE,SmokeNULL,length);
	length=mcudpenumupdate(DPID_BATTERY_STATE,BatteryHig,length);
	length=mcudpenumupdate(DPID_CO_STATE,CONormal,length);
	length=mcudpboolupdate(DPID_MUFFLING,MuteStateOff,length);
	length=mcudpfaultupdate(DPID_FAULT,faultCancel,length);
	length=mcudpvalueupdate(DPID_CO_VALUE,0,length);
	wifi_uart_write_frame(STATE_UPLOAD_CMD,length);
}
#endif
#endif

