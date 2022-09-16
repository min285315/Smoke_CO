/****************************************************************************/
/*	File   :                        Key.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/25,12:13:50                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      SC01                           */
/****************************************************************************/
#include "SceneInstance.h"
#include "Key.h"
#include "KeyAnalysis.h"
#include "main.h"

#define KEYCOUNTERNUMLONG (200)//2000MS
#define KEYCOUNTERNUMSHORT (5)//50MS
#define KEYCOUNTERREPEAT (300)//2S

#define KEY_CHECK_TIME (10)//200MS


struct Key{

	FlagStatus   keyClickFlag;// key press flag
	uint8_t	  CurrentState;//0：short press 1：long press 2:double press   3:three 4: four
	uint8_t	  KeyValue;// key value
	uint16_t	keyCounter;// press counter every 10ms
	uint8_t	  KeyClickState;   // Key Click State flag
};

static KeyTag KeyObject = {.keyClickFlag = RESET,.KeyValue = eKeyNameMax,.CurrentState = 0,.KeyClickState = 0};
static QEvent KeyEvt;
static KeyTag *pKeyObject = NULL;



static eKeyName GetKey(void)
{
	eKeyName Key = eKeyNameMax;
	if (!GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN))
	{
		Key = eKeyPair;
		
	}
	else if (!GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN))//主按�?
	{
		Key = eKeyTest;		
		
	}	
	return (Key);
}

/**
  * @brief  KEY inital  
  * @param  None
  * @retval None
  */
KeyTag * KeyInit(void)
{
	if (NULL == pKeyObject)
	{
		
		GPIO_Init(KEY0_PORT, KEY0_PIN, GPIO_Mode_In_PU_IT); 	//key	
		GPIO_Init(KEY1_PORT, KEY1_PIN, GPIO_Mode_In_PU_IT); 	//key			
		EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);//设置外部中断7 为下降沿有效 
		EXTI_SetPinSensitivity(EXTI_Pin_3,EXTI_Trigger_Falling);//设置外部中断3 为下降沿有效 
		
		TmrInsertTimer(GetTmrHd(), TMR_PERIOD(KEY_CHECK_TIME), PollingKey, (void*)GetKey);
		pKeyObject = &KeyObject;
	}
		
	return pKeyObject;
}

void KeyCheck(void)
{	
	if (!GPIO_ReadInputDataBit(KEY0_PORT, KEY0_PIN)
		||(!GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN))
		&&(KeyObject.keyClickFlag==RESET))
	{
		KeyObject.keyClickFlag = SET;	
    	SetSysSleepFlag(eSysActiveFlag_KeyPress);
	}		
}

static void KeyExitScan(void)
{
	KeyObject.keyClickFlag = RESET;
	ResetSysSleepFlag(eSysActiveFlag_KeyPress);
}
/**********************************************************************/
//Description:		KeyDoubleCheck for polling double click														  
//Parameters:       eHandle   for key value               												  
//Return:   										
//Date:  
//author:		    song
/**********************************************************************/

static void KeyDoubleCheck(TmrHdl Handle, eTmrCmd eCmd)
{
	uint8_t KeyValue;
	KeyTag *pstKey = &KeyObject;;
	KeyValue = (uint32_t)TmrGetCtx(Handle)&0xff;
	switch(eCmd)
	{
		case eTimeOut:
			switch ((pstKey->KeyClickState))
			{
				case 1:
				pstKey->CurrentState = TYPE_SHORT_CLICK;
				pstKey->KeyValue =KeyValue;	
					break;

				case 2:
					pstKey->CurrentState = TYPE_DOUBLE_CLICK;
					pstKey->KeyValue =KeyValue;
					break;

				case 3:
					pstKey->CurrentState = TYPE_THREE_CLICK;
					pstKey->KeyValue =KeyValue;
					break;

				case 4:
					pstKey->CurrentState = TYPE_FOUR_CLICK;
					pstKey->KeyValue =KeyValue;
					break;
				default:
					pstKey->KeyValue =eKeyNameMax;
				break;				
			}
			pstKey->KeyClickState = 0;							
			
			TmrDeleteTimer(Handle);
			ResetSysSleepFlag(eSysActiveFlag_KeyWait);
			break;
			
		default:
			break;
	}

}

static void KeyScanValue(void)
{
	KeyInfo Key = GetAndCleanSysKeyInfo();
	KeyTag *pstKey = &KeyObject;
	if (pstKey->keyClickFlag)
	{
		pstKey->keyCounter = Key.Counter;
		
		if (Key.Counter>=KEYCOUNTERNUMLONG)
		{
			  if ((Key.Counter-KEYCOUNTERNUMLONG)%KEYCOUNTERREPEAT==0)
			  {	
				  pstKey->KeyValue = Key.KeyName;
			  }
			  else
			  {
				  pstKey->KeyValue = eKeyNameMax;
			  }
			  pstKey->CurrentState = TYPE_LONG_CLICK;
		}else 
		{
			  if ((Key.Counter>=KEYCOUNTERNUMSHORT)&&(Key.KeyStat == eKeyStatRelease)&&(Key.Counter<KEYCOUNTERNUMLONG))
			  {
					pstKey->KeyValue = Key.KeyName;
				    pstKey->KeyClickState ++;
					
					if(NULL==TmrGetHandleByEvent(GetTmrHd(),KeyDoubleCheck))
					{
					  	TmrInsertTimer(GetTmrHd(), TMR_PERIOD(1000), KeyDoubleCheck, (void*)(pstKey->KeyValue));
						SetSysSleepFlag(eSysActiveFlag_KeyWait);
					}else
					{
						TmrResetCurCnt(TmrGetHandleByEvent(GetTmrHd(),KeyDoubleCheck));
						
					}
						
					pstKey->KeyValue = eKeyNameMax;
			  }else
			  {
					pstKey->KeyValue = eKeyNameMax;
			  }
		}

		if((Key.KeyStat==eKeyStatRelease))
		{
			KeyExitScan();//next track
		}
	}			  
}

void KeyProcess(void)
{
	KeyTag *pstKey = &KeyObject;
	static uint8_t CurrentStatelast = 0;//for key long press release  0:no long press 1:long press

	KeyCheck(); 
	KeyScanValue();
	 
	if (pstKey->KeyValue != eKeyNameMax)
	{
		switch(pstKey->KeyValue)
		{	
			/* 配网�?*/
			case eKeyPair:
			{
				switch ((pstKey->CurrentState))
				{
					case TYPE_LONG_CLICK:						//长按
						if(CurrentStatelast!=1)
						{
							CurrentStatelast = 1;
							KeyEvt.sig = Q_eKeyEventKey0LongClik;
							SystemPostFifo(&KeyEvt);
						}
					break;
						
					case TYPE_SHORT_CLICK:						//短按
						KeyEvt.sig = Q_eKeyEventKey0Clik;
						SystemPostFifo(&KeyEvt);
						
					break;

					case TYPE_FOUR_CLICK:						//四击
						KeyEvt.sig = Q_eKeyEventKey0FourClik;
						SystemPostFifo(&KeyEvt);
									
					break;
					
					default:
						break;				
					
				}
			}
			break;
			/* 测试�?*/
			case eKeyTest:
			{
				switch ((pstKey->CurrentState))
				{
					
					case TYPE_LONG_CLICK:						//长按
						if(CurrentStatelast!=1)
						{   
							KeyEvt.sig = Q_eKeyEventKey1LongClik;
							CurrentStatelast = 1;
							SystemPostFifo(&KeyEvt);
						}
						break;
						
					case TYPE_SHORT_CLICK:
						KeyEvt.sig = Q_eKeyEventKey1Clik;
						SystemPostFifo(&KeyEvt);
						break;
						
				
					default:
						break;				
						
				}
			
			}
			break;
			
			default:
				break;
		}
		//printf("process KeyValue : %d\n",pstKey->KeyValue);
		KeyObject.KeyValue = eKeyNameMax;
	}
	else
	{
		if((pstKey->keyClickFlag == RESET)&&(CurrentStatelast == 1))//long press release
		{
			CurrentStatelast = 0;
			KeyEvt.sig = Q_eKeyEventKey1LongClikRelease;			
			SystemPostFifo(&KeyEvt);
			
		}else
		{

		}

	}
	
}



