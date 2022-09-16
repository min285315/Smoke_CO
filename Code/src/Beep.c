/****************************************************************************/
/*	File   :             Beep.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/25,12:22:14                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "Beep.h"
#include "Polling.h"
#include "SceneInstance.h"
#include "main.h"

#define BEEP_PORT                           GPIOC
#define BEEP_PIN                            GPIO_Pin_5
#define BEEP_ON()                           GPIO_SetBits(BEEP_PORT, BEEP_PIN)
#define BEEP_OFF()                          GPIO_ResetBits(BEEP_PORT, BEEP_PIN)

struct Beep{
	uint8_t State;
	GPIO_TypeDef *pPort;
	uint8_t pin;
};


static BeepTag BeepObject = {eBeepOff,BEEP_PORT,BEEP_PIN};
static BeepTag *pBeepObject = NULL;


BeepTag * BeepInit(void)
{

	GPIO_Init(BeepObject.pPort, BeepObject.pin, GPIO_Mode_Out_PP_Low_Slow);     //beep
	GPIO_ResetBits(BeepObject.pPort, BeepObject.pin);                             //beep off
	BeepObject.State = eBeepOff;
	pBeepObject = &BeepObject;
	return pBeepObject;	
}

void BeepSetState(eBeepState State)
{
	BeepTag *pBeep = &BeepObject;

	switch (State)
	{
	case eBeepOn:
		GPIO_SetBits(pBeep->pPort, pBeep->pin); 
		break;
	case eBeepOff:
		GPIO_ResetBits(pBeep->pPort, pBeep->pin); 
		break;
	default:
		break;
		
	}
	pBeep->State = State;
}


static void PollingBeepOff(TmrHdl Handle, eTmrCmd eCmd)
{
	switch(eCmd)
	{
		case eTimeOut:
			BeepSetState(eBeepOff);
			TmrDeleteTimer(Handle);
		    ResetSysSleepFlag(eActiveFlagBeep);
			break;
			
		default:
			break;
	}
	//printf("%s \n ",__FUNCTION__);
} 

void BeepPollingHandle(uint32_t Time)
{
	BeepSetState(eBeepOn);
	TmrInsertTimer(GetTmrHd(), TMR_PERIOD(Time), PollingBeepOff, NULL);
	SetSysSleepFlag(eActiveFlagBeep);
	//printf("%s \n ",__FUNCTION__);
}




