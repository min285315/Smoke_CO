/****************************************************************************/
/*	File   :             F:\COproject\hc32l136j8ta_template\example\source\System\SceneInstance.c               */
/*	Author :                       songjiang                                */
/*	Version:                          V1.0                                  */
/*	Date   :                    2019/4/03,11:0:57                         */
/********************************Modify History******************************/
/*   yy/mm/dd     F. Lastname    Description of Modification                */
/****************************************************************************/
/*	Attention:                                                              */
/*	CopyRight:                CopyRight 2017--2025                          */
/*           Shenzhen Xiaojun Technology Co., Ltd.  Allright Reserved        */
/*	Project :                      COPROJECT                                */
/****************************************************************************/
//#include "ArrayOperation.h"
//#include "KeyAnalysis.h"
#include "SceneInstance.h"
#include "main.h"

//#define SYSTEMTICKS    ((uint32_t)0xffffffff)

static TmrHeader gTmrHeader;
static TmrHeader gTmrHeaderIdle;


static uint32_t GetTicks(void)
{
	uint32_t Ret = 0;
	//to be done
	static uint32_t lastTick10ms = 0;
	//uint32_t  tick = pSystemApiTagObj->SystemTick; 
	uint32_t  tick 	= SystemGetTick();
	uint32_t  dTick = (tick>=lastTick10ms)?(tick-lastTick10ms):(SYSTEMTICKS-lastTick10ms+tick);
	if (dTick < 1)
	{	
		Ret = 0;
	}
	else
	{	//10ms

		Ret = dTick;
		lastTick10ms = tick;
	}

	//to be done
	return (Ret);
}

#define SYSTEMIDLETICKS    SYSTEM_ONE_DAY_COUNT
static uint32_t GetIdleTicks(void)
{
	uint32_t Ret = 1;
	static uint32_t lastTick = 0;
	//to be done
	uint32_t  tick = SystemGetIdleNUm();	
	uint32_t  dTick = (tick>=lastTick)?(tick-lastTick):(SYSTEMIDLETICKS-lastTick+tick);
	if (dTick < 1)
	{	
		Ret = 0;
	}
	else
	{	//10s

		Ret = dTick;
		lastTick = tick;
	}
	//to be done
	return (Ret);
}

void PollingModuleInit(void)
{
	
	static TmrInfo TimerMbs[TIMER_MAX_NUM];
	static TmrInfo TimerIdleMbs[TIMER_MAX_NUM];

	TmrInitHeader(&gTmrHeader, GetTicks, TimerMbs, ARRAY_MBS(TimerMbs));
	//to be done,
	TmrInitHeader(&gTmrHeaderIdle, GetIdleTicks, TimerIdleMbs, ARRAY_MBS(TimerIdleMbs));
	//to be done
}

TmrHeader* GetTmrHd(void)
{
	
	return &gTmrHeader;
	
}

TmrHeader* GetTmrIdleHd(void)
{

	return &gTmrHeaderIdle;

}

void SystemPolling(void)
{
	Polling(GetTmrHd());
}

void SystemPollingIdle(void)
{
	Polling(GetTmrIdleHd());
}




