/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: protocol.c
**描        述: 下发/上报数据处理函数
**使 用 说 明 :

                  *******非常重要，一定要看哦！！！********

** 1、用户在此文件中实现数据下发/上报功能
** 2、DP的ID/TYPE及数据处理函数都需要用户按照实际定义实现
** 3、当开始某些宏定义后需要用户实现代码的函数内部有#err提示,完成函数后请删除该#err
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

#include "wifi.h"
#include "main.h"
#include "co.h"
#include "smoke.h"
//#include "qep.h"

 static QEvent WIFIEvt;
/******************************************************************************
                                移植须知:
1:MCU必须在while中直接调用mcu_api.c内的wifi_uart_service()函数
2:程序正常初始化完成后,建议不进行关串口中断,如必须关中断,关中断时间必须短,关中断会引起串口数据包丢失
3:请勿在中断/定时器中断内调用上报函数
******************************************************************************/

         
/******************************************************************************
                              第一步:初始化
1:在需要使用到wifi相关文件的文件中include "wifi.h"
2:在MCU初始化中调用mcu_api.c文件中的wifi_protocol_init()函数
3:将MCU串口单字节发送函数填入protocol.c文件中uart_transmit_output函数内,并删除#error
4:在MCU串口接收函数中调用mcu_api.c文件内的uart_receive_input函数,并将接收到的字节作为参数传入
5:单片机进入while循环后调用mcu_api.c文件内的wifi_uart_service()函数
******************************************************************************/

/******************************************************************************
                        1:dp数据点序列类型对照表
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/
const DOWNLOAD_CMD_S download_cmd[] =
{
  {DPID_SMOKE_SENSOR_STATE, DP_TYPE_ENUM},
  {DPID_CHECKING_RESULT, DP_TYPE_ENUM},
  {DPID_FAULT, DP_TYPE_FAULT},
//  {DPID_LIFECYCLE, DP_TYPE_BOOL},
  {DPID_BATTERY_STATE, DP_TYPE_ENUM},
  {DPID_MUFFLING, DP_TYPE_BOOL},
  {DPID_CO_STATE, DP_TYPE_ENUM},
  {DPID_CO_VALUE, DP_TYPE_VALUE},
};


/******************************************************************************
                           2:串口单字节发送函数
请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
******************************************************************************/

/*****************************************************************************
函数名称 : uart_transmit_output
功能描述 : 发数据处理
输入参数 : value:串口收到字节数据
返回参数 : 无
使用说明 : 请将MCU串口发送函数填入该函数内,并将接收到的数据作为参数传入串口发送函数
*****************************************************************************/
void uart_transmit_output(unsigned char value)
{
    //#error "请将MCU串口发送函数填入该函数,并删除该行"
    UartSendData(value);
	/*
	  //示例:
	  extern void Uart_PutChar(unsigned char value);
	  Uart_PutChar(value);	                                //串口发送函数
	*/
}
/******************************************************************************
                           第二步:实现具体用户函数
1:APP下发数据处理
2:数据上报处理
******************************************************************************/

/******************************************************************************
                            1:所有数据上报处理
当前函数处理全部数据上报(包括可下发/可上报和只上报)
  需要用户按照实际情况实现:
  1:需要实现可下发/可上报数据点上报
  2:需要实现只上报数据点上报
此函数为MCU内部必须调用
用户也可调用此函数实现全部数据上报
******************************************************************************/

//自动化生成数据上报函数

/*****************************************************************************
函数名称 : all_data_update
功能描述 : 系统所有dp点信息上传,实现APP和muc数据同步
输入参数 : 无
返回参数 : 无
使用说明 : 此函数SDK内部需调用;
           MCU必须实现该函数内数据上报功能;包括只上报和可上报可下发型数据
*****************************************************************************/
void all_data_update(void)
{
  //#error "请在此处理可下发可上报数据及只上报数据示例,处理完成后删除该行"
  /* 
  //此代码为平台自动生成，请按照实际数据修改每个可下发可上报函数和只上报函数 
  mcu_dp_enum_update(DPID_SMOKE_SENSOR_STATE,SmokeNULL); //枚举型数据上报;
  mcu_dp_enum_update(DPID_CHECKING_RESULT,SelfTestOK); //枚举型数据上报;
  mcu_dp_fault_update(DPID_FAULT,TRUE); //故障型数据上报;
  mcu_dp_bool_update(DPID_LIFECYCLE,TRUE); //BOOL型数据上报;
  mcu_dp_enum_update(DPID_BATTERY_STATE,BatteryHig); //枚举型数据上报;
  mcu_dp_bool_update(DPID_MUFFLING,FALSE); //BOOL型数据上报;*/

}

/*****************************************************************************
函数名称 : dp_record_combine_update
功能描述 : 记录型数据组合上报
输入参数 : time     : 时间数据长度7,首字节表示是否传输标志位，其余依次为年、月、日、时、分、秒
           dp_bool  : bool型dpid号, v_bool:对应值
           dp_enum  : enum型dpid号, v_enum:对应值
           dp_value : value型dpid号, v_value:对应值
           dp_string: string型dpid号, v_string:对应值,len:string长度
返回参数 : 无
*****************************************************************************/
unsigned char dp_record_combine_update(unsigned char time[],
                                       unsigned char dp_bool,unsigned char v_bool,
                                       unsigned char dp_enum,unsigned char v_enum,
                                       unsigned char dp_value,unsigned long v_value,
                                       unsigned char dp_string,unsigned char v_string[],unsigned char len)
{
  unsigned short length = 0;
  
  if(stop_update_flag == ENABLE)
    return SUCCESS;
  
  //local_time
  length = set_wifi_uart_buffer(length,(unsigned char *)time,7);
  
  //bool
  length = set_wifi_uart_byte(length,dp_bool);
  length = set_wifi_uart_byte(length,DP_TYPE_BOOL);
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,1);
  if(v_bool == FALSE)
  {
    length = set_wifi_uart_byte(length,FALSE);
  }
  else
  {
    length = set_wifi_uart_byte(length,1);
  }
  //enum
  length = set_wifi_uart_byte(length,dp_enum);
  length = set_wifi_uart_byte(length,DP_TYPE_ENUM);
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,1);
  length = set_wifi_uart_byte(length,v_enum);
  //value
  length = set_wifi_uart_byte(length,dp_value);
  length = set_wifi_uart_byte(length,DP_TYPE_VALUE);
  length = set_wifi_uart_byte(length,0);
  length = set_wifi_uart_byte(length,4);
  length = set_wifi_uart_byte(length,v_value >> 24);
  length = set_wifi_uart_byte(length,v_value >> 16);
  length = set_wifi_uart_byte(length,v_value >> 8);
  length = set_wifi_uart_byte(length,v_value & 0xff);
  //string
  length = set_wifi_uart_byte(length,dp_string);
  length = set_wifi_uart_byte(length,DP_TYPE_STRING);
  length = set_wifi_uart_byte(length,len / 0x100);
  length = set_wifi_uart_byte(length,len % 0x100);
  length = set_wifi_uart_buffer(length,(unsigned char *)v_string,len);
  
  wifi_uart_write_frame(STATE_RC_UPLOAD_CMD,length);
  
  return SUCCESS;
}

/******************************************************************************
                                WARNING!!!    
                            2:所有数据下发处理
自动化代码模板函数,具体请用户自行实现数据处理
******************************************************************************/


/*****************************************************************************
函数名称 : dp_download_muffling_handle
功能描述 : 针对DPID_MUFFLING的处理函数
输入参数 : value:数据源数据
        : length:数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERROR
使用说明 : 可下发可上报类型,需要在处理完数据后上报处理结果至app
*****************************************************************************/
//extern void SmokeMuteCmd(void);
//extern void SmokeMuteCancel(void);
static unsigned char dp_download_muffling_handle(const unsigned char value[], unsigned short length)
{
	unsigned char ret;
	//0:关/1:开
	unsigned char muffling;
	muffling=value[0];
//  muffling = mcu_get_dp_download_bool(value,length);
  printf("muffling:%d\n",muffling);
  if(muffling == 0)
  {
  }
  else
  {
 	if((GetSmokeState()==eSmokeState_Alarm)||(GetCOState()==eCOed))
	{
		WIFIEvt.sig=eWIFINETEventMute;  
	}
  }
  SystemPostFifo(&WIFIEvt);
  //处理完DP数据后应有反馈
  ret = mcu_dp_bool_update(DPID_MUFFLING,muffling);
  if(ret == SUCCESS)
    return SUCCESS;
  else
    return ERROR;
}


/******************************************************************************
                                WARNING!!!                     
此代码为SDK内部调用,请按照实际dp数据实现函数内部数据
******************************************************************************/
#ifdef SUPPORT_MCU_RTC_CHECK
/*****************************************************************************
函数名称 : mcu_write_rtctime
功能描述 : MCU校对本地RTC时钟
输入参数 : 无
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void mcu_write_rtctime(unsigned char time[])
{
  //#error "请自行完成RTC时钟写入代码,并删除该行"
  /*
  time[0]为是否获取时间成功标志，为 0 表示失败，为 1表示成功
  time[1] 为 年 份 , 0x00 表 示2000 年
  time[2]为月份，从 1 开始到12 结束
  time[3]为日期，从 1 开始到31 结束
  time[4]为时钟，从 0 开始到23 结束
  time[5]为分钟，从 0 开始到59 结束
  time[6]为秒钟，从 0 开始到59 结束
  time[7]为星期，从 1 开始到 7 结束，1代表星期一
 */
  if(time[0] == 1)
  {
    //正确接收到wifi模块返回的本地时钟数据 
	 
  }
  else
  {
  	//获取本地时钟数据出错,有可能是当前wifi模块未联网
  }
}
#endif

#ifdef WIFI_TEST_ENABLE
/*****************************************************************************
函数名称 : wifi_test_result
功能描述 : wifi功能测试反馈
输入参数 : result:wifi功能测试结果;0:失败/1:成功
           rssi:测试成功表示wifi信号强度/测试失败表示错误类型
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void wifi_test_result(unsigned char result,unsigned char rssi)
{
//	void CalibrationErrIndex(void);
//	void CalibrationOKIndex(void);
	printf("rssi = %d\n",rssi);

  //#error "请自行实现wifi功能测试成功/失败代码,完成后请删除该行"
  if(result == 0)
  {
    //测试失败
    if(rssi == 0x00)
    {
      //未扫描到名称为tuya_mdev_test路由器,请检查
    }
    else if(rssi == 0x01)
    {
      //模块未授权
    }
	CalibrationErrIndex();
  }
  else
  {
    //测试成功
    //rssi为信号强度(0-100, 0信号最差，100信号最强)
    
	if(rssi>75)
	{
		CalibrationOKIndex();
	}else
	{
		CalibrationErrIndex();
	}
    
  }  
}
#endif

/******************************************************************************
                                WARNING!!!                     
以下函数用户请勿修改!!
******************************************************************************/

/*****************************************************************************
函数名称 : dp_download_handle
功能描述 : dp下发处理函数
输入参数 : dpid:DP序号
           value:dp数据缓冲区地址
           length:dp数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERRO
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
  /*********************************
  当前函数处理可下发/可上报数据调用                    
  具体函数内需要实现下发数据处理
  完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
  ***********************************/
  unsigned char ret = 0;
  switch(dpid)
  {
    case DPID_MUFFLING:
      //消音处理函数
      ret = dp_download_muffling_handle(value,length);
      break;

  default:
    break;
  }
  return ret;
}
/*****************************************************************************
函数名称 : get_download_cmd_total
功能描述 : 获取所有dp命令总和
输入参数 : 无
返回参数 : 下发命令总和
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char get_download_cmd_total(void)
{
  return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}
