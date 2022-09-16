/****************************************************************************/
/*	File   :             Beep.h               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/25,12:15:10                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/

#ifndef __BEEP_H__
#define __BEEP_H__

#include"main.h"

typedef enum{
  	eBeepOn,
  	eBeepOff,
}eBeepState;


/**
 * @brief BEEP
 */

#define BEEP_TURN_ON()			{GPIO_SetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_TURN_OFF()			{GPIO_ResetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_TOGGLE()			{GPIO_ToggleBits(BEEP_PORT, BEEP_PIN);}

typedef struct Beep BeepTag;


BeepTag * BeepInit(void);
void BeepSetState(eBeepState State);
void BeepPollingHandle(uint32_t Time);



#endif

