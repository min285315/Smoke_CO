/****************************************************************************/
/*	File   :             Key.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/25,12:13:12                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __KEY_H__
#define __KEY_H__

/**
 * @brief Key push-button
 */
#define KEY0_PORT				GPIOA
#define KEY0_PIN				GPIO_Pin_7

#define KEY1_PORT				GPIOE
#define KEY1_PIN				GPIO_Pin_3

typedef enum{
	TYPE_LONG_CLICK = 0x00,
	TYPE_SHORT_CLICK,
	TYPE_DOUBLE_CLICK,
	TYPE_THREE_CLICK,
	TYPE_FOUR_CLICK,
	
}KEYType;



typedef struct Key KeyTag;


KeyTag * KeyInit(void);
void KeyCheck(void);

void KeyProcess(void);


#endif

