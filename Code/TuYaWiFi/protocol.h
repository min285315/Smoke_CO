/****************************************Copyright (c)*************************
**                               版权所有 (C), 2015-2020, 涂鸦科技
**
**                                 http://www.tuya.com
**
**--------------文件信息-------------------------------------------------------
**文   件   名: protocol.h
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
#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_



/******************************************************************************
                            用户相关信息配置
******************************************************************************/
/******************************************************************************
                            1:修改产品信息                
******************************************************************************/
// cfet7ouwznslditf   w92ank4gj1cakf7w --Old
// xs33zvw7zd3d1zaf   -- SC240-WT-CB3S
// cfet7ouwznslditf    --sc240-wt-WE3S
#ifdef SoftWareVer_SC240_WT_CB3S_EN
#define PRODUCT_KEY "xs33zvw7zd3d1zaf"    //开发平台创建产品后生成的16位字符产品唯一标识
#else
#define PRODUCT_KEY "cfet7ouwznslditf"    //开发平台创建产品后生成的16位字符产品唯一标识
#endif
#define MCU_VER "1.0.0"                                 //用户的软件版本,用于MCU固件升级,MCU升级版本需修改
/******************************************************************************
                         2:定义收发缓存:
                    如当前使用MCU的RAM不够,可修改为24
******************************************************************************/
#ifndef SUPPORT_MCU_FIRM_UPDATE
#define WIFI_UART_QUEUE_LMT             128//16              //数据接收队列大小,如MCU的RAM不够,可缩小
#define WIFI_UART_RECV_BUF_LMT          128              //根据用户DP数据大小量定,必须大于24
#else
#define WIFI_UART_QUEUE_LMT             256             //数据接收队列大小,单包选择256，队列默认128，可以根据实际情况适当调整（过小可能会影响ota下载）
//#define WIFI_UART_QUEUE_LMT             256             //数据接收队列大小,单包选择512，队列默认256，可以根据实际情况适当调整（过小可能会影响ota下载）
//#define WIFI_UART_QUEUE_LMT             512             //数据接收队列大小,单包选择1024，队列默认512，可以根据实际情况适当调整（过小可能会影响ota下载）

//请在此处选择合适的mcu升级缓存大小（根据上面固件包选择大小来选择开启多大的mcu升级缓存）
#define WIFI_UART_RECV_BUF_LMT            256             //固件升级缓冲区,需大缓存,如单包大小选择256，则缓存必须大于260
//#define WIFI_UART_RECV_BUF_LMT          600             //固件升级缓冲区,需大缓存,如单包大小选择512，则缓存必须大于520
//#define WIFI_UART_RECV_BUF_LMT          1200            //固件升级缓冲区,需大缓存,如单包大小选择1024，则缓存必须大于1030
#endif
    
#define WIFIR_UART_SEND_BUF_LMT         128              //根据用户DP数据大小量定,必须大于48


/******************************************************************************
                      3:MCU是否需要支持校时功能                     
如需要请开启该宏,并在Protocol.c文件内mcu_write_rtctime实现代码
mcu_write_rtctime内部有#err提示,完成函数后请删除该#err
mcu在wifi模块正确联网后可调用mcu_get_system_time()函数发起校时功能
******************************************************************************/
//#define         SUPPORT_MCU_RTC_CHECK                //开启校时功能

/******************************************************************************
                      4:MCU是否需要支持wifi功能测试                     
如需要请开启该宏,并且mcu在需要wifi功能测试处调用mcu_api.c文件内mcu_start_wifitest
并在protocol.c文件wifi_test_result函数内查看测试结果,
wifi_test_result内部有#err提示,完成函数后请删除该#err
******************************************************************************/
#define         WIFI_TEST_ENABLE                //开启WIFI产测功能

/******************************************************************************
                          5:MCU是否需要支持MCU固件升级                  
如需要支持MCU固件升级,请开启该宏
                        ********WARNING!!!**********
当前接收缓冲区为关闭固件更新功能的大小,固件升级包可选择，默认256字节大小
如需要开启该功能,串口接收缓冲区会变大

******************************************************************************/
//#define         SUPPORT_MCU_FIRM_UPDATE                 //开启MCU固件升级功能(默认关闭)
//固件包大小选择
#ifdef SUPPORT_MCU_FIRM_UPDATE
#define PACKAGE_SIZE                   0        //包大小为256字节
//#define PACKAGE_SIZE                 1        //包大小为512字节
//#define PACKAGE_SIZE                 2        //包大小为1024字节
#endif

/******************************************************************************
                          5:MCU是否支持wifi模块固件升级              
这里 WIFI 模块的电源通断是由 MCU 去控制的，这里当我们 MCU 需要去升级 WIFI
模块的固件便可以通过下面的命令去拉去最新固件。MCU 主板根据 WIFI 模块的回复包决
定是否需要给 WIFI 模块断电。这里 MCU 当发送 0a 命令等待 5S 没有收到相关回复便把
WIFI 模块断电。当模块回复正在更新固件这里 MCU 也需要起一个定时当处于正在更新固
件超过 60S 没有收到固件升级成功，也强制认为固件升级失败给 WIFI 模块断电

如需要支持wifi模块固件升级,请开启该宏
******************************************************************************/
//#define         WIFI_MODULE_UPDATE                 //开启WIFI模块固件升级

/******************************************************************************
                          6:MCU是否支持获取dp缓存指令
对于某些带设置性或控制功能的传感器，需要新增 dp 下发功能。当在设备离线时，面板
有指令下发，这条指令会缓存在云端，等待设备主动获取。缓存命令的为增量方式，对于
已经获取过的命令，再次获取时不会下发

如需要支持获取dp缓存指令,请开启该宏
******************************************************************************/
//#define         DO_CACHE_SUPPORT                   //获取dp缓存指令

/******************************************************************************
                        1:dp数据点序列号重新定义
          **此为自动生成代码,如在开发平台有相关修改请重新下载MCU_SDK**         
******************************************************************************/
//烟雾检测状态(只上报)
//备注:【必选】
//1：烟雾报警
//2:烟雾报警解除
typedef enum{
	SmokeIs = 0,
	SmokeNULL = 1,
	SmokeMax = 0xff,
}WiFieSmokeState;
#ifdef SoftWareVer_SC240_WT_CB3S_EN
#define DPID_SMOKE_SENSOR_STATE 1 
#elif (defined SoftWareVer_SC240_WT_EN)
#define DPID_SMOKE_SENSOR_STATE 13
#endif

typedef enum{
	SelfTesting = 0,
	SelfTestOK,
	SelfTestERR,
	SelfTestMax = 0xff,
}eTestSelf;
//设备自检结果(只上报)
//备注:【非必选】
//1:自检中
//2:自检成功
//3:自检失败；
//4:特殊情况:面板自检超时
//自检时，建议面板强制等待自检结果，自检拿不到新的结果返回，显示超时。如果面板新进入时，拿到的自检状态值为“自检中”并且是x 秒内上报的，面板显示为自检中状态


#ifdef SoftWareVer_SC240_WT_CB3S_EN
#define DPID_CHECKING_RESULT 9
#elif (defined SoftWareVer_SC240_WT_EN)
#define DPID_CHECKING_RESULT 19
#endif


//bit0:fault bit1:serious_fault bit2:sensor_fault bit3:probe_fault bit4:power_fault

typedef enum{
	faultCancel = 0x00,
	fault = 0x01,
	serious_fault=0x02,
	sensor_fault=0x04,
	probe_fault= 0x08,
	power_fault=0x10
}eFAULTStateWIFI;


//故障告警(只上报)
//备注:【非必选】

#ifdef SoftWareVer_SC240_WT_CB3S_EN
#define DPID_FAULT 11
#elif (defined SoftWareVer_SC240_WT_EN)
#define DPID_FAULT 21
#endif


typedef enum{
	BatteryLow = 0,
	BatteryMid,
	BatteryHig,
	BatteryMax = 0xff
}eBatteryStateWIFI;

//电池电量状态(只上报)
//备注:低、中、高 三种状态，低电定义为电量值少于30%，将触发低电报警

#ifdef SoftWareVer_SC240_WT_CB3S_EN
#define DPID_BATTERY_STATE 14
#elif (defined SoftWareVer_SC240_WT_EN)
#define DPID_BATTERY_STATE 2
#endif


typedef enum{
	MuteStateOff = 0,
	MuteStateOn,
	MuteStateMax = 0xff,
}eMuteState;

//消音(可下发可上报)
//备注:硬件开始发出报警声后，上报该dp点。app可以通过指令下发，停止设备在本周期内的报警

#ifdef SoftWareVer_SC240_WT_CB3S_EN
#define DPID_MUFFLING 16  
#elif (defined SoftWareVer_SC240_WT_EN)
#define DPID_MUFFLING 24  
#endif


//CO检测值(只上报)
//备注:

#ifdef SoftWareVer_SC240_WT_CB3S_EN
#define DPID_CO_VALUE 19
#elif (defined SoftWareVer_SC240_WT_EN)
#define DPID_CO_VALUE 35
#endif


//CO检测状态(只上报)
//备注:

#ifdef SoftWareVer_SC240_WT_CB3S_EN
#define DPID_CO_STATE 18
#elif (defined SoftWareVer_SC240_WT_EN)
#define DPID_CO_STATE 34
#endif

typedef enum{
	COAlarm = 0,
	CONormal,
}eCoalarmState;


/*****************************************************************************
函数名称 : all_data_update
功能描述 : 系统所有dp点信息上传
输入参数 : 无
返回参数 : 无
使用说明 : MCU必须实现该函数内数据上报功能
*****************************************************************************/
void all_data_update(void);

/*****************************************************************************
函数名称 : dp_record_combine_update
功能描述 : 记录型数据组合上报
输入参数 : time     : 时间数据数,首字节表示是否传输标志位，其余依次为年、月、日、时、分、秒
           dp_bool  : bool型dpid号, v_bool:对应值
           dp_enum  : enum型dpid号, v_enum:对应值
           dp_value : value型dpid号, v_value:对应值
           dp_string: string型dpid号, v_string:对应值,len:string长度
返回参数 : 无
*****************************************************************************/
unsigned char dp_record_combine_update(unsigned char time[],
                                       unsigned char dp_bool,unsigned char v_bool,
                                       unsigned char dp_enum,unsigned char v_enum,
                                       unsigned char dp_value, unsigned long v_value,
                                       unsigned char dp_string,unsigned char v_string[],unsigned char len);

#ifdef SUPPORT_MCU_RTC_CHECK
/*****************************************************************************
函数名称 : mcu_write_rtctime
功能描述 : MCU校对本地RTC时钟
输入参数 : 无
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void mcu_write_rtctime(unsigned char time[]);
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
void wifi_test_result(unsigned char result,unsigned char rssi);
#endif

#ifdef WIFI_MODULE_UPDATE
/*****************************************************************************
函数名称 : qur_module_ug_result
功能描述 : 请求 WIFI 模块固件升级结果返回
输入参数 : result:请求 WIFI 模块固件升级结果;
           0x00(开始检测固件更新）不可断电
           0x01（已经是最新固件） 断电
           0x02(正在更新固件）不可断电
           0x03(固件更新成功）断电
           0x04(固件更新失败）断电
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void qur_module_ug_result(unsigned char result);
#endif

/*****************************************************************************
函数名称 : router_strenth_result
功能描述 : 查询当前连接路由器信号强度
输入参数 : result:wifi功能测试结果;0:失败/1:成功
           rssi:测试成功表示wifi信号强度/测试失败表示错误类型
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void router_strenth_result(unsigned char result,unsigned char rssi);

#ifdef SUPPORT_MCU_FIRM_UPDATE
/*****************************************************************************
函数名称 : qur_ug_result
功能描述 : 请求 MCU 固件升级
输入参数 : result:请求 WIFI 模块固件升级结果;
           0x00(开始检测固件更新）不可断电
           0x01（已经是最新固件） 断电
           0x02(正在更新固件）不可断电
           0x03(固件更新成功）断电
           0x04(固件更新失败）断电
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void qur_ug_result(unsigned char result);

/*****************************************************************************
函数名称 : upgrade_package_choose
功能描述 : 升级包大小选择
输入参数 : package_sz:升级包大小
           0x00：默认 256byte
           0x01：512byte 
           0x02：1024byte

返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
void upgrade_package_choose(unsigned char package_sz);

/*****************************************************************************
函数名称 : mcu_firm_update_handle
功能描述 : MCU进入固件升级模式
输入参数 : value:固件缓冲区
           position:当前数据包在于固件位置
           length:当前固件包长度(固件包长度为0时,表示固件包发送完成)
返回参数 : 无
使用说明 : MCU需要自行实现该功能
*****************************************************************************/
unsigned char mcu_firm_update_handle(const unsigned char value[],unsigned long position,unsigned short length);
#endif





/*****************************************************************************
函数名称 : dp_download_handle
功能描述 : dp下发处理函数
输入参数 : dpid:DP序号
value:dp数据缓冲区地址
length:dp数据长度
返回参数 : 成功返回:SUCCESS/失败返回:ERRO
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length);
/*****************************************************************************
函数名称 : get_download_cmd_total
功能描述 : 获取所有dp命令总和
输入参数 : 无
返回参数 : 下发命令总和
使用说明 : 该函数用户不能修改
*****************************************************************************/
unsigned char get_download_cmd_total(void);

#endif

