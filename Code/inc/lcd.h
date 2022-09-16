/**
  ******************************************************************************
  * @file    lcd.h
  * @author  zhenrong.peng
  * @version V1.0
  * @date    25-05-2017
  * @brief   This file contains definitions for HT1621B
  *          
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H
#define __LCD_H

/* Includes ------------------------------------------------------------------*/
//#include "Framework.h"
#include "main.h"



#define DISPLAY_CLEAR   (0x00)
#define DISPLAY_ALL     (0xFF)

/**
 * @brief lcd
 */
#define LCD_BACK_LIGHT_PORT                 GPIOF//
#define LCD_BACK_LIGHT_PIN                  GPIO_Pin_0// 

#define LCD_POWER_PORT                      GPIOD//GPIOE
#define LCD_POWER_PIN                       GPIO_Pin_7// 

#define LCD_BACK_LIGHT_ON()                 GPIO_SetBits(LCD_BACK_LIGHT_PORT, LCD_BACK_LIGHT_PIN)
#define LCD_BACK_LIGHT_OFF()                GPIO_ResetBits(LCD_BACK_LIGHT_PORT, LCD_BACK_LIGHT_PIN)
//#define LCD_POWER_ON()                      GPIO_SetBits(LCD_POWER_PORT, LCD_POWER_PIN)
//#define LCD_POWER_OFF()                     GPIO_ResetBits(LCD_POWER_PORT, LCD_POWER_PIN)


/* Exported Macros ------------------------------------------------------------*/               




/* Exported types ------------------------------------------------------------*/

#define ASCII_NUM_0         ((uint8_t)0x30)
#define LCD_DIS_NULL  		(ASCII_NUM_0+10)
#define LCD_DIS_LINE  		(ASCII_NUM_0+11)
#define LCD_DIS_A  			(ASCII_NUM_0+12)
#define LCD_DIS_B  			(ASCII_NUM_0+13)
#define LCD_DIS_L  			(ASCII_NUM_0+14)
#define LCD_DIS_E  			(ASCII_NUM_0+15)
#define LCD_DIS_R  			(ASCII_NUM_0+16)
#define LCD_DIS_P  			(ASCII_NUM_0+17)
#define LCD_DIS_S  			(ASCII_NUM_0+18)


#define  LCD_START (0)
#define  LCD_END   (2)


typedef enum 
{
//BATTERY
	LCD_BATTERY_COM = 3, 
	LCD_BATTERY_LEVEL1_COM = 1, 
	LCD_BATTERY_LEVEL2_COM = 2, 
	LCD_BATTERY_LEVEL3_COM = 0,
	
	LCD_CO_COM = 0,
	LCD_CO_PPM_COM = 0,

	//LCD_GAS_COM = 1,
	LCD_PEAK_COM = 0,
	LCD_WIFI_COM0 = 0,
	LCD_WIFI_COM1 = 1,
	LCD_WIFI_COM2 = 2,

	LCD_OUT_PWR_COM = 3,

    LCD_TWO_FLOAT_COM = 0,
	LCD_ONE_FLOAT_COM = 0,
	LCD_PERCENT_COM = 0,
	
	LCD_DIS_LINE_COM =2,

	
	LCD_DIS_0_COM =0,
	LCD_DIS_1_COM =1,
	LCD_DIS_2_COM =2,
	LCD_DIS_3_COM =3,


} LCDPointSCOMTag;


typedef enum 
{
	LCD_BATTERY_SEG = 7, 
	LCD_BATTERY_LEVEL1_SEG = 7, 
	LCD_BATTERY_LEVEL2_SEG = 7, 
	LCD_BATTERY_LEVEL3_SEG = 7,
	
	LCD_CO_SEG = 2,
	LCD_CO_PPM_SEG = 0,

	//LCD_GAS_SEG = 6,
	LCD_PEAK_SEG = 4,
	LCD_OUT_PWR_SEG = 6,
	LCD_WIFI_SEG = 6,
	
	LCD_TWO_FLOAT_SEG = 4,
	LCD_ONE_FLOAT_SEG = 2,
	LCD_PERCENT_SEG = 6,

	
	LCD_DIS_LINE1_SEG =4,
	LCD_DIS_LINE2_SEG =2,
	LCD_DIS_LINE3_SEG =0,
	/*
	LCD_DIS_LINE1_SEG =6,
	LCD_DIS_LINE2_SEG =2,
	LCD_DIS_LINE3_SEG =7,
	*/
	LCD_DIS_2A_SEG =3,
	LCD_DIS_2B_SEG =2,
	LCD_DIS_2C_SEG =2,
	LCD_DIS_2D_SEG =3,
	LCD_DIS_2E_SEG =3,
	LCD_DIS_2F_SEG =3,
	LCD_DIS_2G_SEG =2,
	
	LCD_DIS_3A_SEG =1,
	LCD_DIS_3B_SEG =0,
	LCD_DIS_3C_SEG =0,
	LCD_DIS_3D_SEG =1,
	LCD_DIS_3E_SEG =1,
	LCD_DIS_3F_SEG =1,
	LCD_DIS_3G_SEG =0,
	
} LCDPointSEGTag;


  
typedef enum{
  eLcdOFF,
  eLcdON,
  eLcdNULL,
  eLcdUpdate,
  eLcdBackLightOFF,
  eLcdBackLightON,

}eLcdState;


typedef enum{
	eStr_End,
	eStr_Err,
	eStr_LINE,
	eStr_Lb,
	eStr_PAS,
	eStr_E06,
	eStr_E07,
	eStr_E08,
	eStr_E09,
	eStr_E10,
	eStr_E11,
	eStr_E12,
}eLcdStr;


typedef enum{
	eLcdPointOFF,
	eLcdPointON
}eLcdPointState;



typedef enum{
  eLcdBatteryFlgLevel_Null,
  eLcdBatteryFlgLevel_1,
  eLcdBatteryFlgLevel_2,
  eLcdBatteryFlgLevel_3,
  eLcdBatteryFlgOff,
}eLcdBatteryFlgLevel;


typedef struct Lcd LcdTag;



/* Exported Functions ------------------------------------------------------------*/
LcdTag * LcdInit(void);
void LCD_Control(eLcdState state);
void LCD_Clear(void);
void LCD_DisplayNum(uint16_t num);
void LCD_DsiplayCoFlg(eLcdState control);
void LCD_DisplayCoData(uint16_t num);
void LCD_DisplayBatteryData(uint8_t level);
void LCD_SystemStartAnimation(uint8_t line);
void LCD_BackLightControl(eLcdState state);
void LCD_DsiplayStr(eLcdStr str, uint8_t start,uint8_t end);
void LCDStandyDisplay(void);
void LcdDisplayPoint(eLcdPointState eType,uint8_t com,uint8_t seg);




#endif /* __LCD_H */




/************************END OF FILE***********************/


