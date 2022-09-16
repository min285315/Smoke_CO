/**
  ******************************************************************************
  * @file    led.h
  * @author  zhenrong.peng
  * @version V1.0
  * @date    01-04-2017
  * @brief   Header for led.c
  *****************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    LED_TYPE_RED=0x00,
    LED_TYPE_GREEN,
    LED_TYPE_YELLOW, 
    LED_TYPE_MAX, 
	LED_TYPE_ESCAPE,
}eLed_Type;

typedef enum
{
    LED_OFF=0x00,
    LED_ON,
    LED_TOGGLE,
}eLed_State;	
typedef struct Led LedTag;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/**
 * @brief LED
 */

//#define YELLOW_LED_PORT                     GPIOE
//#define YELLOW_LED_PIN                      GPIO_Pin_3
#define GREEN_LED_PORT                      GPIOE
#define GREEN_LED_PIN                       GPIO_Pin_1
#define RED_LED_PORT                        GPIOE
#define RED_LED_PIN                         GPIO_Pin_0
//#define ESCAPE_LIGHT_PORT                   GPIOA               //same with backlight pin
//#define ESCAPE_LIGHT_PIN                    GPIO_Pin_3



#define R_LED_TURN_ON()			{GPIO_SetBits(R_LED_PORT, R_LED_PIN);}
#define R_LED_TURN_OFF()		{GPIO_ResetBits(R_LED_PORT, R_LED_PIN);}
#define R_LED_TOGGLE()			{GPIO_ToggleBits(R_LED_PORT, R_LED_PIN);}

#define G_LED_TURN_ON()			{GPIO_SetBits(G_LED_PORT, G_LED_PIN);}
#define G_LED_TURN_OFF()		{GPIO_ResetBits(G_LED_PORT, G_LED_PIN);}
#define G_LED_TOGGLE()			{GPIO_ToggleBits(G_LED_PORT, G_LED_PIN);}

#define Y_LED_TURN_ON()			{GPIO_SetBits(R_LED_PORT, R_LED_PIN); GPIO_SetBits(G_LED_PORT, G_LED_PIN);}
#define Y_LED_TURN_OFF()		{GPIO_ResetBits(R_LED_PORT, R_LED_PIN); GPIO_ResetBits(G_LED_PORT, G_LED_PIN);}
#define Y_LED_TOGGLE()			{GPIO_ToggleBits(R_LED_PORT, R_LED_PIN); GPIO_ToggleBits(G_LED_PORT, G_LED_PIN);}



#define GREEN_LED_ON()                      GPIO_SetBits(GREEN_LED_PORT, GREEN_LED_PIN)
#define GREEN_LED_OFF()                     GPIO_ResetBits(GREEN_LED_PORT, GREEN_LED_PIN)
#define GREEN_LED_TOGGLE()                  GPIO_ToggleBits(GREEN_LED_PORT, GREEN_LED_PIN)
#define RED_LED_ON()                        GPIO_SetBits(RED_LED_PORT, RED_LED_PIN)
#define RED_LED_OFF()                       GPIO_ResetBits(RED_LED_PORT, RED_LED_PIN)
#define RED_LED_TOGGLE()                    GPIO_ToggleBits(RED_LED_PORT, RED_LED_PIN)
#define YELLOW_LED_ON()                     GPIO_SetBits(YELLOW_LED_PORT, YELLOW_LED_PIN)
#define YELLOW_LED_OFF()                    GPIO_ResetBits(YELLOW_LED_PORT, YELLOW_LED_PIN)
#define YELLOW_LED_TOGGLE()                 GPIO_ToggleBits(YELLOW_LED_PORT, YELLOW_LED_PIN)


//#define ESCAPE_LIGHT_ON()                   GPIO_SetBits(ESCAPE_LIGHT_PORT, ESCAPE_LIGHT_PIN)
//#define ESCAPE_LIGHT_OFF()                  GPIO_ResetBits(ESCAPE_LIGHT_PORT, ESCAPE_LIGHT_PIN)

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
LedTag * LedInit(void);
void LedSetState(eLed_Type LedType, eLed_State State);
void LedPollingHandle(eLed_Type LedType, uint32_t Time);
void PollingLedGreen(TmrHdl Handle, eTmrCmd eCmd);



#endif /* __LED_H */



/************************END OF FILE**************************/


