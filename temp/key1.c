#include "main.h"
#include "base.h"
#include "key.h"
#include "uart.h"
#include "SceneInstance.h"





#define KEYCOUNTERNUMLONG 				    (150)		    //3000MS
#define KEYCOUNTERNUMCONTINUOUS_LIMIT 	    (100)			//300MS
#define KEYCOUNTERNUMINTERVAL_LIMIT 		(80)		    //200MS
#define KEYCOUNTERNUMSHORT_LIMIT 		    (70)			//400MS
#define KEYCOUNTERNUMSHORT 				    (2)			    //20MS
#define KEYCOUNTERREPEAT 					(20)		    //20MS

#define KEY_CLICK_1_TIMES					(1)
#define KEY_CLICK_2_TIMES					(2)
#define KEY_CLICK_3_TIMES					(3)
#define KEY_CLICK_4_TIMES					(4)

static QEvent KeyEvt;
static KeyApi KeyApiTag;
static KeyApi* pKeyApiTag = &KeyApiTag;
static TmrHdl DoubleKeyScanTimeoutHeader = NULL;
static TmrHdl FourtimesScanTimeoutHeader = NULL;

void PollingDoubleClickScan(TmrHdl Handle, eTmrCmd eCmd);
void PollingFourtimesClickScan(TmrHdl Handle, eTmrCmd eCmd);

/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void KeyInit(void)
{
	GPIO_Init(KEY0_PORT, KEY0_PIN, GPIO_Mode_In_PU_IT);     //key
	GPIO_Init(KEY1_PORT, KEY1_PIN, GPIO_Mode_In_PU_IT);     //key
	
	EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);//设置外部中断1 为下降沿有效 
	EXTI_SetPinSensitivity(EXTI_Pin_3,EXTI_Trigger_Falling);//设置外部中断1 为下降沿有效 
	pKeyApiTag->keyCounter  = 0;	
	pKeyApiTag->keyClickFlag = 0;
	pKeyApiTag->CurrentState = 0;
	pKeyApiTag->keyValue = KEY_DEFAULT;
	pKeyApiTag->keyCurrent =  KEY_DEFAULT;	
	pKeyApiTag->keyValueLast = KEY_DEFAULT;
	pKeyApiTag->keyType = TYPE_NONE;
}


/**********************************************************************/
//Description:	GetKeyApiTag()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
KeyApi* GetKeyApiTag(void)
{
	return pKeyApiTag;
}

/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
uint8_t GetKeyClickFlag(void)
{
	return pKeyApiTag->keyClickFlag;
}


/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void SetKeyClickFlag(uint8_t Flag)
{
	pKeyApiTag->keyClickFlag = Flag;
}



/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
static uint16_t KeyGpioValue()
{	
	uint16_t ret = KEY_DEFAULT;
	if(KEY0_is_L())
	{
		ret |= KEY0_PRESS;
	}

	if(KEY1_is_L())
	{
		ret |= KEY1_PRESS;
	}

	return ret;
}


/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void KeyCheck(void)
{
	static uint8_t flag = 0 ;
	KeyApi* pKeyApi = GetKeyApiTag();
	if (GetKeyClickFlag())
	{
		flag = 0;
	}
	else
	{
		if (KeyGpioValue() == KEY_DEFAULT)
		{ 
			ResetSysSleepFlag(eSysActiveFlag_KeyPress);
			flag = 1;
			SetKeyClickFlag(0);
			pKeyApi->keyCounter = 0;	
			pKeyApi->keyCurrent = KEY_DEFAULT;
			//key1 release
			if( pKeyApi->CurrentState == 1)
			{
				pKeyApi->CurrentState = 0;
				KeyEvt.sig = Q_eKeyEventKey1LongClikRelease;
				SystemPostFifo(&KeyEvt);
			}
		}
		else
		{
			SetSysSleepFlag(eSysActiveFlag_KeyPress);
			if(flag == 1)
			{
				SetKeyClickFlag(1);
			}
		}
	}
}


/**********************************************************************/
//Description:	Key_Init()															  
//Parameters:                        												  
//Return:   																  
//Date:           quanwu.xu  
/**********************************************************************/
void KeyGpioScan(void)
{
	KeyApi* pKeyApi = GetKeyApiTag();
	uint16_t KeyTemp = KeyGpioValue();
	 
	if(GetKeyClickFlag())
	{
		if (pKeyApi->keyCurrent == KeyTemp)						//按下按键												
		{
			pKeyApi->keyCounter++;
			if(pKeyApi->keyCounter >= KEYCOUNTERNUMLONG)		//10 ms * 300 = 3000ms
			{	
				pKeyApi->keyType = TYPE_LONG_CLICK;
				pKeyApi->keyValue = ( KEYValue)(pKeyApi->keyCurrent & 0x7F);
				SetKeyClickFlag(0);
				
				
			}
		}
		else
		{
			if(KeyTemp == KEY_DEFAULT)
			{
				
				if((pKeyApi->keyCounter >= KEYCOUNTERNUMSHORT)	&& (pKeyApi->keyCounter 
												< KEYCOUNTERNUMSHORT_LIMIT))//短按按键
				{
					pKeyApi->keyType = TYPE_WAIT;
					pKeyApi->keyValue = (KEYValue)(pKeyApi->keyCurrent & 0x7F);
					SetKeyClickFlag(0);					

					if(FourtimesScanTimeoutHeader)
					{	
						pKeyApi->keyClickCnt++;
						if(pKeyApi->keyClickCnt == KEY_CLICK_4_TIMES)
						{
							pKeyApi->keyType = TYPE_FOUR_CLICK;
						}
					}
					else if(DoubleKeyScanTimeoutHeader)
					{
						
						pKeyApi->keyClickCnt++;
						if(pKeyApi->keyClickCnt == KEY_CLICK_2_TIMES)
						{	 
							FourtimesScanTimeoutHeader = TmrInsertTimer(GetTmrHd(),
										TMR_PERIOD(1000), PollingFourtimesClickScan, NULL);
						}
					}
					else
					{	
						DoubleKeyScanTimeoutHeader = TmrInsertTimer(GetTmrHd(),
										TMR_PERIOD(1000), PollingDoubleClickScan, NULL);
					}
				}
				else
				{
					pKeyApi->keyCounter = 0;
					pKeyApi->keyValue = KEY_DEFAULT;
					pKeyApi->keyCurrent = KEY_DEFAULT;
					SetKeyClickFlag(0);
				}		
			}
			else
			{
				pKeyApi->keyCounter  = 0;
				pKeyApi->keyValue = KEY_DEFAULT; 
				pKeyApi->keyCurrent = KEY_DEFAULT;
			}
		}	
		pKeyApi->keyCurrent =  (KEYValue)KeyTemp;
	}
	
	
}


/**********************************************************************/
//Description:	isNewKeyMsg()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Authot:  	quanwu.xu  
/**********************************************************************/
bool isNewKeyMsg(void)
{
	bool ret = FALSE;
	KeyApi* pKeyApi = GetKeyApiTag();
	if ((pKeyApi->keyValue != KEY_DEFAULT) && (pKeyApi->keyType != TYPE_WAIT))
	{
		pKeyApi->keyValueLast = pKeyApi->keyValue;
		pKeyApi->keyValue = KEY_DEFAULT;
		ret = TRUE;
	}
	return ret; 
}


/**********************************************************************/
//Description:	PollingFourtimesClickScan()															  
//Parameters:                        												  
//Return:   																  
//Date:         quanwu.xu  
/**********************************************************************/
void PollingFourtimesClickScan(TmrHdl Handle, eTmrCmd eCmd)
{
	KeyApi* pKeyApi = GetKeyApiTag();
	
    Handle = Handle;
    switch(eCmd)
    {
    	case eInsertTimer:
			pKeyApi->keyClickCnt = KEY_CLICK_2_TIMES;
			SetSysSleepFlag(eSysActiveFlag_KeyWait);
			break;
			
        case eTimeOut:
			if(pKeyApi->keyClickCnt == KEY_CLICK_2_TIMES)
			{
				pKeyApi->keyType = TYPE_DOUBLE_CLICK;
			}
			else if(pKeyApi->keyClickCnt == KEY_CLICK_3_TIMES)
			{
				pKeyApi->keyType = TYPE_THREE_CLICK;
			}
        break;

		case eDelTimer:
			ResetSysSleepFlag(eSysActiveFlag_KeyWait);
			break;
			
        default:
            break;
    }
}


/**********************************************************************/
//Description:	PollingDoubleClickScan()															  
//Parameters:                        												  
//Return:   																  
//Date:         quanwu.xu  
/**********************************************************************/
void PollingDoubleClickScan(TmrHdl Handle, eTmrCmd eCmd)
{
	KeyApi* pKeyApi = GetKeyApiTag();
	
    Handle = Handle;
    switch(eCmd)
    {
    	case eInsertTimer:
			pKeyApi->keyClickCnt = KEY_CLICK_1_TIMES;
			SetSysSleepFlag(eSysActiveFlag_KeyWait);
			break;
			
        case eTimeOut:
			if(pKeyApi->keyClickCnt == KEY_CLICK_1_TIMES)
			{
				pKeyApi->keyType = TYPE_SHORT_CLICK;
			}
        break;

		case eDelTimer:
			ResetSysSleepFlag(eSysActiveFlag_KeyWait);
			break;
			
		default:
			break;
    }
}


/**********************************************************************/
//Description:	PollingKeyScan()															  
//Parameters:                        												  
//Return:   																  
//Date:           quanwu.xu  
/**********************************************************************/
void PollingKeyScan(TmrHdl Handle, eTmrCmd eCmd)
{

    Handle = Handle;
    switch(eCmd)
    {
        case eTimeOut:
        {
        	if(GetKeyClickFlag())
        	{
        		KeyGpioScan();
        	}
        }
        break;

        default:
            break;
    }
}

/**********************************************************************/
//Description:	Key0Handle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void Key0Handle(KEYType KeyType)
{
	switch(KeyType)
	{
		case TYPE_SHORT_CLICK:
			KeyEvt.sig = Q_eKeyEventKey0Clik;
			SystemPostFifo(&KeyEvt);
			break;
			
		case TYPE_LONG_CLICK:
			KeyEvt.sig = Q_eKeyEventKey0LongClik;
			SystemPostFifo(&KeyEvt);
			break;
			
		case TYPE_FOUR_CLICK:
			KeyEvt.sig = Q_eKeyEventKey0FourClik;
			SystemPostFifo(&KeyEvt);
			break;

		default:
			break;
	}
	
}


/**********************************************************************/
//Description:	Key1Handle()															  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:	quanwu.xu  
/**********************************************************************/
void Key1Handle(KEYType KeyType)
{
	switch(KeyType)
	{
		case TYPE_SHORT_CLICK:
			KeyEvt.sig = Q_eKeyEventKey1Clik;
			SystemPostFifo(&KeyEvt);
			break;
			
		case TYPE_LONG_CLICK:
			KeyEvt.sig = Q_eKeyEventKey1LongClik;
			SystemPostFifo(&KeyEvt);
			pKeyApiTag->CurrentState = 1;
			break;

		default:
			break;		
	}
}

/**********************************************************************/
//Description:	Key_Handle()															  
//Parameters:                        												  
//Return:   																  
//Date:         quanwu.xu  
/**********************************************************************/
void KeyHandleMsg(void)
{
	KeyApi *pKeyApi = GetKeyApiTag();
	
	KeyCheck();
	if(isNewKeyMsg() == TRUE)
	{
		switch (pKeyApi->keyValueLast)
		{
			case (KEY0_PRESS)://閰嶅鎸夐敭
			{ 
				
				Key0Handle(pKeyApi->keyType);
			}
			break;

			case (KEY1_PRESS)://娴嬭瘯鎸夐敭
			{ 
				
				Key1Handle(pKeyApi->keyType);
			}
			break;

			default:
				break;
			
		}

		pKeyApi->keyClickCnt = 0;
		pKeyApi->keyType = TYPE_NONE;
		pKeyApi->keyValueLast = KEY_DEFAULT;
		TmrDeleteTimer(DoubleKeyScanTimeoutHeader);
		DoubleKeyScanTimeoutHeader = NULL;
		TmrDeleteTimer(FourtimesScanTimeoutHeader);
		FourtimesScanTimeoutHeader = NULL;
	}

}






