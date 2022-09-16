#ifndef _KEY_H_
#define _KEY_H_
#include "main.h"
#include "polling.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define	KEY0_is_L()	(GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN) == RESET)	//Key Press L
#define	KEY0_is_H()	(GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN) != RESET)	//Key Press H

#define	KEY1_is_L()	(GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == RESET)	//Key Press L
#define	KEY1_is_H()	(GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) != RESET)	//Key Press H

typedef enum{
	KEY0_PRESS = (uint16_t)0x01,
	KEY1_PRESS = (uint16_t)0x02,
	KEY2_PRESS = (uint16_t)0x04,
	KEY3_PRESS = (uint16_t)0x08,
	KEY4_PRESS = (uint16_t)0x10,
	KEY5_PRESS = (uint16_t)0x20,
	KEY6_PRESS = (uint16_t)0x40,
	KEY7_PRESS = (uint16_t)0x80,

	KEY_DEFAULT = (uint16_t)0x8000,
}KEYValue;


typedef enum{
	TYPE_NONE = 0x00,
	TYPE_WAIT,
	TYPE_SHORT_CLICK,
	TYPE_DOUBLE_CLICK,//3
	TYPE_THREE_CLICK,
	TYPE_FOUR_CLICK,
	TYPE_LONG_CLICK,
}KEYType;

typedef struct _KeyApi{
	uint16_t keyCounter;
	uint8_t keyClickFlag;
	KEYType keyType;	
	KEYValue keyCurrent;
	KEYValue keyValue;
	KEYValue keyValueLast;
	uint8_t  keyClickCnt;
	uint8_t  CurrentState ;
}KeyApi;

void KeyInit(void);
void KeyCheckBefore(void);
void PollingKeyScan(TmrHdl Handle, eTmrCmd eCmd);
void KeyHandleMsg(void);

#ifdef __cplusplus
}
#endif



#endif

