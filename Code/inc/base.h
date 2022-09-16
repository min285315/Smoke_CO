#ifndef _BASE_H_
#define _BASE_H_

#include "stm8l15x.h"
#include "main.h"


#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)



#define Temp_ADC_CHANNEL ADC_Channel_18

#define TEMPERATURE_FACTORY_START			(35)//(40)					// 20 C
#define TEMPERATURE_FACTORY_END				(56)//(55)					// 30 C
#define TEMPERATURE_OFFSET					(20)					// 0 C




#define R_LED_PORT				GPIOE
#define R_LED_PIN				GPIO_Pin_0

#define G_LED_PORT				GPIOE
#define G_LED_PIN				GPIO_Pin_1

#define BEEP_PORT				GPIOC
#define BEEP_PIN				GPIO_Pin_5

#define SMOKE_OP_CTR_PORT		GPIOE
#define SMOKE_OP_CTR_PIN		GPIO_Pin_4

#define SMOKE_IR_CTR_PORT		GPIOB
#define SMOKE_IR_CTR_PIN		GPIO_Pin_1
//AN23
#define SMOKE_IR_ADC_PORT		GPIOE
#define SMOKE_IR_ADC_PIN		GPIO_Pin_5

#define SMOKE_TEST_PORT			GPIOB
#define SMOKE_TEST_PIN			GPIO_Pin_2

#define TEMP_CTR_PORT			GPIOD
#define TEMP_CTR_PIN			GPIO_Pin_3

#define TEMP_ADC_PORT			GPIOB
#define TEMP_ADC_PIN			GPIO_Pin_0

#define BAT_CTR_PORT			GPIOD
#define BAT_CTR_PIN				GPIO_Pin_0

#define BAT_ADC_PORT			GPIOD
#define BAT_ADC_PIN				GPIO_Pin_2




#define SMOKE_OP_TURN_ON()		{GPIO_SetBits(SMOKE_OP_CTR_PORT, SMOKE_OP_CTR_PIN);}
#define SMOKE_OP_TURN_OFF()		{GPIO_ResetBits(SMOKE_OP_CTR_PORT, SMOKE_OP_CTR_PIN);}

#define SMOKE_IR_TURN_ON()		{GPIO_SetBits(SMOKE_IR_CTR_PORT, SMOKE_IR_CTR_PIN);}
#define SMOKE_IR_TURN_OFF()		{GPIO_ResetBits(SMOKE_IR_CTR_PORT, SMOKE_IR_CTR_PIN);}

#define SMOKE_TEST_TURN_ON()	{GPIO_SetBits(SMOKE_TEST_PORT, SMOKE_TEST_PIN);}
#define SMOKE_TEST_TURN_OFF()	{GPIO_ResetBits(SMOKE_TEST_PORT, SMOKE_TEST_PIN);}

#define TEMP_DETECT_ON()		{GPIO_SetBits(TEMP_CTR_PORT, TEMP_CTR_PIN);}
#define TEMP_DETECT_OFF()		{GPIO_ResetBits(TEMP_CTR_PORT, TEMP_CTR_PIN);}

#define BAT_DETECT_ON()			{GPIO_SetBits(BAT_CTR_PORT, BAT_CTR_PIN);}
#define BAT_DETECT_OFF()		{GPIO_ResetBits(BAT_CTR_PORT, BAT_CTR_PIN);}


#define ARRAY_MBS(Array) (sizeof(Array)/sizeof(Array[0]))


#define ADC_SAMPLE_TIMES				((uint8_t)0x04)


void Delay(__IO uint16_t TimeCnt);
void DelayMs(__IO uint16_t nTime);
void LsiMeasurment(void);
void RtcWakeUpInit(uint16_t WakeupCount);
void RtcWakeUpReload(uint16_t WakeupCount);

void SysClkConfig(void);
void Timer4Config(void);
void GpioConfig(void);


void RtcSetBaseTime(uint16_t rtc_1s);
uint16_t RtcGetBaseTime(void);


uint16_t GetAdcSample(void);

uint8_t GetTempData(void);
uint8_t SensorTemperatureCheck(void);
uint8_t SensorGetTemperature(void);


#endif

