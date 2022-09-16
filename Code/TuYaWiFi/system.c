/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: system.c
**描        述: wifi数据处理函数
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
#define SYSTEM_GLOBAL
#include "main.h"
#include "wifi.h"
#include "uart.h"
#include "smoke.h"
#include "co.h"

//
#ifdef SUPPORT_WIFI_NET_FUNCTION
extern const DOWNLOAD_CMD_S download_cmd[];
unsigned short firm_size;                                                      //升级包一包的大小

/*****************************************************************************
函数名称 : set_wifi_uart_byte
功能描述 : 写wifi_uart字节
输入参数 : dest:缓存区其实地址;
           byte:写入字节值
返回参数 : 写入完成后的总长度
*****************************************************************************/
unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte)
{
  unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;
  
  *obj = byte;
  dest += 1;
  
  return dest;
}
/*****************************************************************************
函数名称 : set_wifi_uart_buffer
功能描述 : 写wifi_uart_buffer
输入参数 : dest:目标地址
           src:源地址
           len:数据长度
返回参数 : 无
*****************************************************************************/
unsigned short set_wifi_uart_buffer(unsigned short dest, unsigned char *src, unsigned short len)
{
  unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;
  
  my_memcpy(obj,src,len);
  
  dest += len;
  return dest;
}
/*****************************************************************************
函数名称 : wifi_uart_write_data
功能描述 : 向wifi uart写入连续数据
输入参数 : in:发送缓存指针
           len:数据发送长度
返回参数 : 无
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
函数名称 : get_check_sum
功能描述 : 计算校验和
输入参数 : pack:数据源指针
           pack_len:计算校验和长度
返回参数 : 校验和
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
函数名称 : wifi_uart_write_frame
功能描述 : 向wifi串口发送一帧数据
输入参数 : fr_type:帧类型
           len:数据长度
返回参数 : 无
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
函数名称 : heat_beat_check
功能描述 : 心跳包检测
输入参数 : 无
返回参数 : 无
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
函数名称  : product_info_update
功能描述  : 产品信息上传
输入参数 : 无
返回参数 : 无
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
函数名称 : get_update_dpid_index
功能描述 : 或许制定DPID在数组中的序号
输入参数 : dpid:dpid
返回参数 : index:dp序号
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
函数名称 : data_point_handle
功能描述 : 下发数据处理
输入参数 : value:下发数据源指针
返回参数 : ret:返回数据处理结果
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
    //错误提示
    return FALSE;
  }
  else
  {
    ret = dp_download_handle(dp_id,value + 4,dp_len);
  }
  
  return ret;
}
/*****************************************************************************
函数名称 : data_handle
功能描述 : 数据帧处理
输入参数 : offset:数据起始位
返回参数 : 无
*****************************************************************************/
static QEvent WIFIEVENT;

void data_handle(unsigned short offset)
{
#ifdef SUPPORT_MCU_FIRM_UPDATE
      unsigned char *firmware_addr;
      static unsigned long firm_length;                                             //MCU升级文件长度
      static unsigned char firm_update_flag;                                        //MCU升级标志
      unsigned long dp_len;
      unsigned char firm_flag;                                                      //升级包大小标志
#else
  unsigned short dp_len;
#endif
  unsigned char ret;
  unsigned short i,total_len;
  unsigned char cmd_type = wifi_uart_rx_buf[offset + FRAME_TYPE];
  
  switch(cmd_type)
  {
  case HEAT_BEAT_CMD:                                   //心跳包
    heat_beat_check();
    break;
    
  case PRODUCT_INFO_CMD:                                //产品信息
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
  case WIFI_STATE_CMD:                                  //wifi工作状态	
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

  case WIFI_RESET_CMD:                                  //重置wifi(wifi返回成功)
    reset_wifi_flag = RESET_WIFI_SUCCESS;
	
    break;
    
  case WIFI_MODE_CMD:                                   //选择smartconfig/AP模式(wifi返回成功)	
    set_wifimode_flag = SET_WIFICONFIG_SUCCESS;
    break;
#endif
    
  case DATA_QUERT_CMD:                                  //命令下发
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
        //成功提示
      }
      else
      {
        //错误提示
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
  case GET_LOCAL_TIME_CMD:                             //获取本地时间
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
函数名称 : get_queue_total_data
功能描述 : 读取队列内数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
unsigned char get_queue_total_data(void)
{
  if(queue_in != queue_out)
    return 1;
  else
    return 0;
}
/*****************************************************************************
函数名称 : Queue_Read_Byte
功能描述 : 读取队列1字节数据
输入参数 : 无
返回参数 : 无
*****************************************************************************/
unsigned char Queue_Read_Byte(void)
{
  unsigned char value = 0;
  
  if(queue_out != queue_in)
  {
    //有数据
    if(queue_out >= (unsigned char *)(wifi_queue_buf + sizeof(wifi_queue_buf)))
    {
      //数据已经到末尾
      queue_out = (unsigned char *)(wifi_queue_buf);
    }
    
    value = *queue_out ++;   
  }
  
  return value;
}

#endif
