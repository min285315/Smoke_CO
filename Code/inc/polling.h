#ifndef _POLLING_H_
#define _POLLING_H_

#include "base.h"

#define TICK_UNIT 10 //ms

//unit of Period is ms
#define TMR_PERIOD(Period) \
	((Period)<TICK_UNIT? 1: (((Period)+TICK_UNIT/2)/TICK_UNIT))

#define TICK_IDLE_UNIT (SLEEP_TIME) //10s

//unit of Period is ms
#define TMR_IDLE_PERIOD(Period) \
	((Period)<TICK_IDLE_UNIT? 1: (((Period)+TICK_IDLE_UNIT/2)/TICK_IDLE_UNIT))


typedef void* TmrHdl;

typedef enum _eTimerCmd
{
	eTimeOut,
	eInsertTimer,
	eDelTimer,
	eTimerBlock,
	eTmrCmdMax,
}eTmrCmd;

typedef void (*pfTmrCBK)(TmrHdl Handle, eTmrCmd eCmd);

typedef struct _TimerInfo
{
	pfTmrCBK 	pfEvent;
	void* 		pCtx;
	uint32_t 		CurCnt;
	uint32_t 		ReloadCnt;
}TmrInfo;

typedef uint32_t (*pfTimimg)(void);

typedef struct _TimerInfoHeader
{
	TmrInfo* 	pTmrInfo;
	uint32_t    TmrMbs;
	pfTimimg 	pfGetTicks;
}TmrHeader;


extern TmrHeader* TmrInitHeader(TmrHeader* pHeader, pfTimimg pfGetTicks,
								TmrInfo* pTimer, uint32_t Mbs);
extern TmrHdl TmrInsertTimer(TmrHeader* pHeader, uint32_t ReloadCnt, 
								pfTmrCBK pfEvent, void* pCtx);
extern void TmrDeleteTimer(TmrHdl Handle);
extern void Polling(TmrHeader* pHeader);

extern void* TmrGetCtx(TmrHdl Handle);
extern void TmrSetCurCnt(TmrHdl Handle, uint32_t CurCnt);
extern void TmrResetCurCnt(TmrHdl Handle);
extern uint32_t TmrGetMbs(const TmrHeader* pHeader);
extern TmrHdl TmrGetHandleByEvent(const TmrHeader* pHeader,
											pfTmrCBK pfEvent);



#endif

