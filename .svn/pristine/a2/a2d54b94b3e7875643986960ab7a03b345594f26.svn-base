#ifndef _UART_H_
#define _UART_H_
#include "stdio.h"


void UsartConfig(void);
void PrintfVoltage(uint16_t AdcValue);

void DMAConfigForUart(void);
void UartHandleRevData(void);
void UartSendData(uint8_t data);

#define  PrintVol(Temp) \
do{\
	printflog("%d", Temp/1000);\
	printflog(".%d", Temp%1000/100);\
	printflog("%d", Temp%100/10);\
	printflog("%d", Temp%10);\
}while(0)


#define SUPPORT_DEBUG_MODE
#ifdef SUPPORT_DEBUG_MODE
#define printflog(fmt,args...) printf(fmt, ##args)
#else
#define printflog(fmt,args...) /*do nothing */
#endif

#endif

