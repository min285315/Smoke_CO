
#ifndef __SYS_CFG_H
#define __SYS_CFG_H


/***************************************************软件版本******************************************/
/*独立款-软件宏定义*/
//#define SoftWareVer_SC200_EN	
//#define SoftWareVer_SC200_UL

/*WIFI款-软件宏定义*/

#define SoftWareVer_SC240_WT_EN	
//#define SoftWareVer_SC240_WT_CB3S_EN	


/*RF款-软件宏定义*/
//#define SoftWareVer_SC200_WR_EN	
//#define SoftWareVer_SC200_WR_UL	

//调试信息
//#define Debug_Test
//#define DEBUG_NET


//补偿
#define SUPPORT_SmokeTempCompensation	//smoke温度补偿
#define SUPPORT_CoTempCompensation		//Co温度补偿



#define SoftWareVer  "1.4.0" 
		
		
//传感器模块开关		
#define SUPPORT_CO_FUNCTION
#define SUPPORT_SMOKE_FUNCTION
#define SUPPORT_LCD_FUNCTION

//#define GETPEAK


/*独立款*/
#ifdef  SoftWareVer_SC200_EN
		#define SUPPORT_EN50291_EN14604							//EN标准
		#define DeviceName	"SC200"
		#define SMOKE_ALARM_LEVEL 				((uint16_t)320)//烟感灵敏度阈值
/*WT*/
#elif  (defined SoftWareVer_SC240_WT_EN)
		#define SUPPORT_EN50291_EN14604							//EN标准
//		#define SUPPORT_UL2034_UL217							//UL标准
		#define DeviceName	"SC240-WT-WE3S"
		#define SMOKE_ALARM_LEVEL 				((uint16_t)300)//烟感灵敏度阈值	
		#define SUPPORT_UART_DMA_FUNCTION
		#define SUPPORT_WIFI_NET_FUNCTION

#elif  (defined SoftWareVer_SC240_WT_CB3S_EN)
		#define SUPPORT_EN50291_EN14604							//EN标准
//		#define SUPPORT_UL2034_UL217							//UL标准
		#define DeviceName	"SC240-WT-CB3S"
		#define SMOKE_ALARM_LEVEL 				((uint16_t)410)//烟感灵敏度阈值	
		#define SUPPORT_UART_DMA_FUNCTION
		#define SUPPORT_WIFI_NET_FUNCTION

#elif	(defined SoftWareVer_SC220_UL)
		#define SUPPORT_UL2034_UL217							//UL标准
		#define DeviceName	"SC200"
		#define SMOKE_ALARM_LEVEL 				((uint16_t)320)//烟感灵敏度阈值

/*RF款*/		
#elif   (defined SoftWareVer_SC200_WR_EN)
		#define SUPPORT_EN50291_EN14604							//EN标准
		#define SUPPORT_NET_FUNCTION
		#define SUPPORT_UART_DMA_FUNCTION
		#define SUPPORT_RF_NET_FUNCTION
		#define DeviceName	"SC200_WR"
		//烟感灵敏度阈值
		#define SMOKE_ALARM_LEVEL 				((uint16_t)320)
#elif   (defined SoftWareVer_SC200_WR_UL)
		#define SUPPORT_UL2034_UL217							//UL标准
		#define SUPPORT_NET_FUNCTION
		#define SUPPORT_UART_DMA_FUNCTION
		#define SUPPORT_RF_NET_FUNCTION
		#define DeviceName	"SC200_WR"
		//烟感灵敏度阈值
		#define SMOKE_ALARM_LEVEL 				((uint16_t)320)



#endif
































#endif /* __SYS_CFG_H */




/************************END OF FILE***********************/




