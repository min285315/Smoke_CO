#ifndef _CO_H_
#define _CO_H_

#include "main.h"

//co Check State
#define CO_NORMAL_CHECK  ((uint8_t)0)
#define CO_OPEN_FAULT_CHECK   ((uint8_t)1)
#define CO_SHORT_FAULT_CHECK   ((uint8_t)2)

//co sensor
#define CO_DETECT_ADC_CHANNEL               ADC_Channel_3

#define CO_SHORT_PORT                      GPIOE
#define CO_SHORT_PIN                       GPIO_Pin_6

#define CO_DETECT_SHORT_ON()                {GPIO_SetBits(CO_SHORT_PORT,CO_SHORT_PIN);}  
#define CO_DETECT_SHORT_OFF()               {GPIO_ResetBits(CO_SHORT_PORT,CO_SHORT_PIN);}



//co alarm set
#define CO_CALI_STATE                       (0xa5)
#define CO_ALARM_400PPM_CONCENTRATION       (340)//400
#define CO_ALARM_150PPM_CONCENTRATION       (120)//150
#define CO_ALARM_70PPM_CONCENTRATION        (56)//70

#define CO_ALARM_70PPM_TIME        			(120)        // 120min    (60-240)
#define CO_ALARM_150PPM_TIME       			(20)         // 20min    (10-50)
#define CO_ALARM_400PPM_TIME       			(6)          // 6min     (4-15)

#define CO_ALARM_300PPM_CONCENTRATION       (210)//300
#define CO_ALARM_100PPM_CONCENTRATION       (70)//100
#define CO_ALARM_50PPM_CONCENTRATION        (40)//50
#define CO_ALARM_30PPM_CONCENTRATION        (22)//30

#define CO_ALARM_30PPM_TIME        			(150)         // 150min    (>120)
#define CO_ALARM_50PPM_TIME        			(70)          // 70min     (60-90)
#define CO_ALARM_100PPM_TIME       			(20)         // 20min      (10-40)
#define CO_ALARM_300PPM_TIME       			(1)          // 1min       (<3)

//合德co 开路数值
#define SENSOR_OPEN_FAULT_SAMPLE                1800//2100//2300//2.3v
#define SENSOR_OPEN_SAMPLE                      600//600mv
#define SENSOR_SHORT_SAMPLE_MAX                 2000//2600//2.6v
#define SENSOR_SHORT_SAMPLE_MIN                 1000//1v

//CO校机 合格数值
#define PPM_CARIBRATON_STEP0_THRESHOLD_MIN          (80)      // mv 
#define PPM_CARIBRATON_STEP0_THRESHOLD_MAX          (250)//300     // mv 

#define PPM_CARIBRATON_STEP1_THRESHOLD_MIN          (150)      // mv 
#define PPM_CARIBRATON_STEP1_THRESHOLD_MAX          (500)//600     // mv   


#if defined SUPPORT_UL2034_UL217
#define PPM_CARIBRATON_STEP2_THRESHOLD_MIN          (30)      // mv 
#define PPM_CARIBRATON_STEP2_THRESHOLD_MAX          (50)      // mv

#elif defined SUPPORT_EN50291_EN14604
#define PPM_CARIBRATON_STEP2_THRESHOLD_MIN          (23)      // mv 
#define PPM_CARIBRATON_STEP2_THRESHOLD_MAX          (37)//(50)      // mv       
#else

#endif



//co data
#define     CO_TN_20   1000 
#define     CO_TN_19   1000 
#define     CO_TN_18   1000 
#define     CO_TN_17   1000 
#define     CO_TN_16   1000 
#define     CO_TN_15   1000 
#define     CO_TN_14   1000 
#define     CO_TN_13   1000 
#define     CO_TN_12   1000 
#define     CO_TN_11   1000

#define     CO_TN_10   1000
#define     CO_TN_9    1000 
#define     CO_TN_8    1000 
#define     CO_TN_7    1000 
#define     CO_TN_6    1000 
#define     CO_TN_5    1000  
#define     CO_TN_4    1000
#define     CO_TN_3    1000  
#define     CO_TN_2    1000  
#define     CO_TN_1    1000

#define     CO_TP_0    1000

#define     CO_TP_1    1000  
#define     CO_TP_2    1000  
#define     CO_TP_3    1000     
#define     CO_TP_4    1000  
#define     CO_TP_5    1000 
#define     CO_TP_6    1000 
#define     CO_TP_7    1000 
#define     CO_TP_8    1000 
#define     CO_TP_9    1000 
#define     CO_TP_10   1000 

#define     CO_TP_11   1000 
#define     CO_TP_12   1000 
#define     CO_TP_13   1000 
#define     CO_TP_14   1000 
#define     CO_TP_15   1000 
#define     CO_TP_16   1000
#define     CO_TP_17   1000 
#define     CO_TP_18   1000 
#define     CO_TP_19   1000 
#define     CO_TP_20   1000 

#define     CO_TP_21   1000 
#define     CO_TP_22   1000
#define     CO_TP_23   1000 
#define     CO_TP_24   1000 
#define     CO_TP_25   1000 
#define     CO_TP_26   1000
#define     CO_TP_27   1000 
#define     CO_TP_28   1000
#define     CO_TP_29   1000 
#define     CO_TP_30   1000 

#define     CO_TP_31   1000 
#define     CO_TP_32   1000 
#define     CO_TP_33   1000 
#define     CO_TP_34   1000 
#define     CO_TP_35   1000 
#define     CO_TP_36   1000 
#define     CO_TP_37   1000 
#define     CO_TP_38   1000 
#define     CO_TP_39   1000 
#define     CO_TP_40   1000 

#define     CO_TP_41   1000
#define     CO_TP_42   1000 
#define     CO_TP_43   1000
#define     CO_TP_44   1000 
#define     CO_TP_45   1000 
#define     CO_TP_46   1000 
#define     CO_TP_47   1000
#define     CO_TP_48   1000
#define     CO_TP_49   1000 
#define     CO_TP_50   1000 

#define     CO_TP_51   1000 
#define     CO_TP_52   1000 
#define     CO_TP_53   1000 
#define     CO_TP_54   1000
#define     CO_TP_55   1000 
#define     CO_TP_56   1000
#define     CO_TP_57   1000 
#define     CO_TP_58   1000
#define     CO_TP_59   1000 
#define     CO_TP_60   1000
#define     CO_TP_61   1000 
#define     CO_TP_62   1000
#define     CO_TP_63   1000 
#define     CO_TP_64   1000 
#define     CO_TP_65   1000
#define     CO_TP_66   1000 
#define     CO_TP_67   1000 
#define     CO_TP_68   1000
#define     CO_TP_69   1000 
#define     CO_TP_70   1000


#if defined SUPPORT_UL2034_UL217
#define COPPMLEVEL   (0)
#define COPPMLEVEL0  (30)
#define COPPMLEVEL1  (70)
#define COPPMLEVEL2  (120)
#define COPPMLEVEL3  (340)
#define COPPMLEVEL4  (5000)
#elif defined SUPPORT_EN50291_EN14604
#define COPPMLEVEL   (0)
#define COPPMLEVEL0  (30)
#define COPPMLEVEL1  (50)
#define COPPMLEVEL2  (70)
#define COPPMLEVEL3  (210)
#define COPPMLEVEL4  (4500)
#endif


typedef enum{
	eCONO = 0,
	eCOed,// co alarm
	eCOErr,
	eCOOPEN,
	eCOSHORT,
	
}eCOState;

typedef enum{
	LEVEL = 0,
	LEVEL0,
	LEVEL1,
	LEVEL2,
	LEVEL3,
	LEVEL4,
	LEVEL5,
}eCOLEVEL;

typedef struct CO COTag;


void COInit(void);

void SysCoCailModeHandleEvent(SystemApiTag  *me, QEvent *e);

void COSelfCheck(void);

void COKeyTestDelay(uint8_t Flag);
void SystemCOAlarm(SystemApiTag  *me,QEvent *e);
void SystemCOError(SystemApiTag  *me,QEvent *e);
void COLcdDisplay(void);
uint16_t GetCOPpm(void);
FlagStatus IsCOPPM(void);
uint8_t GetCOState(void);
void COSetPeakData(uint16_t PpmPeakValue);
uint16_t COGetPeakData(void);
bool IsUartOpen();


#ifdef SUPPORT_RF_NET_FUNCTION
void NetCOAlarm(SystemApiTag  *me,QEvent *e);


#endif


#endif

