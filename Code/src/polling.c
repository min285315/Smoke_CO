#include "polling.h"


TmrHeader* TmrInitHeader(TmrHeader* pHeader, pfTimimg pfGetTicks,
							TmrInfo* pTimer, uint32_t Mbs)
{
	if((NULL==pHeader) || (NULL==pfGetTicks) || (NULL==pTimer) || (0==Mbs))
	{
		return (NULL);
	}

	pHeader->pTmrInfo = pTimer;
	pHeader->TmrMbs = Mbs;
	pHeader->pfGetTicks = pfGetTicks;

	while(Mbs > 0)
	{
		pTimer->pfEvent = NULL;
		pTimer++;
		Mbs--;
	} 

	return (pHeader);
}


TmrHdl TmrInsertTimer(TmrHeader* pHeader, uint32_t ReloadCnt, 
							pfTmrCBK pfEvent, void* pCtx)
{

	uint32_t Mbs= 0;
	TmrInfo* pTimer = NULL;
	TmrInfo* pTheTimer = NULL;
	
	if((NULL==pHeader) || (NULL==pfEvent))
	{
		return (NULL);
	}

	pTimer = pHeader->pTmrInfo;
	Mbs = pHeader->TmrMbs;

	while(Mbs > 0)// has it this event ?
	{
		if(pTimer->pfEvent == pfEvent)// has this event

		{
			pTheTimer = NULL;
			//printf("Timer is have\n");
			break;
		}else  // has other event
		{
			pTimer++;
			Mbs--;
		}

	}

	if (0 == Mbs)//has null event

	{
		pTimer = pHeader->pTmrInfo;
		Mbs = pHeader->TmrMbs;
		//printf("Timer is none,Mbs=%ld\n",Mbs);
		while(Mbs > 0)// has it this event ?
		{

			if(pTimer->pfEvent == NULL)// has null event 
			{
				pTheTimer = pTheTimer==NULL? pTimer: pTheTimer;
				break;
			}else  // has other event
			{
				pTimer++;
				Mbs--;
			}			

		}		

	}



	if (Mbs == 0)
	{
		//printf("Timer is Over Err\n");
	}else
	{
		//printf("Timer has %ld,%ld\n",Mbs-1,(uint32_t)pfEvent);
	}

	if(pTheTimer != NULL)

	{
		ReloadCnt = ReloadCnt>0? ReloadCnt: 1; 
		pTheTimer->pCtx = pCtx;
		pTheTimer->CurCnt = ReloadCnt;
		pTheTimer->ReloadCnt = ReloadCnt;
		pTheTimer->pfEvent = pfEvent;
		pfEvent((TmrHdl)pTheTimer, eInsertTimer);
	}


	return ((TmrHdl)pTheTimer);

}



void TmrDeleteTimer(TmrHdl Handle)
{
	TmrInfo* pTimer = (TmrInfo*)Handle;
	pfTmrCBK pfEvent = NULL;
	
	if(pTimer != NULL)
	{
		pfEvent = pTimer->pfEvent ;
		
		if(pfEvent != NULL)
		{
			pTimer->pfEvent = NULL;
			pfEvent(Handle, eDelTimer);
		}
	}
}

static void CheckTimeOut(TmrInfo* pTimer, uint32_t Ticks)
{
	const pfTmrCBK pfEvent = pTimer->pfEvent;
	uint32_t CurCnt = 0;

	if(pfEvent != NULL)
	{
		CurCnt = pTimer->CurCnt;
		//printf("Ticks = %d,CurCnt = %d\n",(int)Ticks,(int)CurCnt);
		if(CurCnt >= Ticks)
		{
			CurCnt -= Ticks;

			if(CurCnt != 0)
			{
				pTimer->CurCnt = CurCnt;
				//printf("v--\n");
			}
			else
			{
				pTimer->CurCnt = pTimer->ReloadCnt;
				pfEvent((TmrHdl)pTimer, eTimeOut);
				//printf("reset v1\n");
			}
		}
		else
		{
			pTimer->CurCnt = pTimer->ReloadCnt;
			pfEvent((TmrHdl)pTimer, eTimerBlock);
			pfEvent((TmrHdl)pTimer, eTimeOut);
			//printf("reset v2\n");
		}
	}
}

void Polling(TmrHeader* pHeader)
{
	uint32_t Mbs = 0;
	TmrInfo* pTimer = NULL;
	uint32_t Ticks = 0;
	
	if(NULL == pHeader)
	{
		return;
	}

	Ticks = pHeader->pfGetTicks();
	
	if(Ticks > 0)
	{
		pTimer = pHeader->pTmrInfo;
		Mbs = pHeader->TmrMbs;
		
		while(Mbs > 0)
		{
			CheckTimeOut(pTimer, Ticks);
			pTimer++;
			Mbs--;
		}
	}
}

void* TmrGetCtx(TmrHdl Handle)
{
	void* pCtx = NULL;

	if(Handle != NULL)
	{
		pCtx = ((TmrInfo*)Handle)->pCtx;
	}

	return (pCtx);
}

void TmrSetCurCnt(TmrHdl Handle, uint32_t CurCnt)
{
	TmrInfo* pTimer = (TmrInfo*)Handle;
	
	if(pTimer != NULL)
	{
		pTimer->CurCnt = CurCnt;
	}
}

void TmrResetCurCnt(TmrHdl Handle)
{
	TmrInfo* pTimer = (TmrInfo*)Handle;
	
	if(pTimer != NULL)
	{
		pTimer->CurCnt = pTimer->ReloadCnt;
		//printf("TmrResetCurCnt = %ld\n",pTimer->CurCnt);
	}
}

uint32_t TmrGetMbs(const TmrHeader* pHeader)
{
	uint32_t Mbs = 0;

	if(NULL != pHeader)
	{
		Mbs = pHeader->TmrMbs;
	}

	return (Mbs);
}

TmrHdl TmrGetHandleByEvent(const TmrHeader* pHeader, pfTmrCBK pfEvent)
{
	uint32_t Mbs= 0;
	TmrInfo* pTimer = NULL;

	if((NULL==pHeader) || (NULL==pfEvent))
	{
		return (NULL);
	}

	pTimer = pHeader->pTmrInfo;
	Mbs = pHeader->TmrMbs;

	while(Mbs > 0)
	{
		if(pTimer->pfEvent == pfEvent)
		{
			break;
		}

		pTimer++;
		Mbs--;
	}
	if (Mbs==0)//防止timer数组溢出
	{
		pTimer = NULL;	
	}
	return ((TmrHdl)pTimer);
}


