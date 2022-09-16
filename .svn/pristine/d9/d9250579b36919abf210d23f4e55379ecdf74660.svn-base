#include "main.h"
#include "base.h"
#include "uart.h"
#include "system.h"


#define USART1_Tx_PORT                      GPIOC
#define USART1_Rx_PORT                      GPIOC
#define USART1_Tx_Pin                       GPIO_Pin_3
#define USART1_Rx_Pin                       GPIO_Pin_2

#ifdef _RAISONANCE_
#define PUTCHAR_PROTOTYPE int putchar (char c)
#define GETCHAR_PROTOTYPE int getchar (void)
#elif defined (_COSMIC_)
#define PUTCHAR_PROTOTYPE char putchar (char c)
#define GETCHAR_PROTOTYPE char getchar (void)
#else /* _IAR_ */
#define PUTCHAR_PROTOTYPE int putchar (int c)
#define GETCHAR_PROTOTYPE int getchar (void)
#endif /* _RAISONANCE_ */


/* Private define ------------------------------------------------------------*/
#define UART_DR_ADDRESS        ((uint16_t)0x5231)		//UART寄存器地址
#define UART_BAUDRATE        	((uint32_t)9600)
#define UART_RX_BUFFER_LENGTH   (255)


/* Private variables ---------------------------------------------------------*/
uint8_t  RxBuffer[UART_RX_BUFFER_LENGTH];


PUTCHAR_PROTOTYPE
{
	/* Write a character to the USART */
	USART_SendData8(USART1, c);
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

	return (c);
}
/**
  * @brief Retargets the C library scanf function to the USART.
  * @param[in] None
  * @retval char Character to Read
  * @par Required preconditions:
  * - None
  */
GETCHAR_PROTOTYPE
{
	int c = 0;
	/* Loop until the Read data register flag is SET */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	c = USART_ReceiveData8(USART1);
	return (c);
}

void UsartConfig(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
	USART_DeInit(USART1);
	
	GPIO_Init(USART1_Tx_PORT, USART1_Tx_Pin, GPIO_Mode_Out_PP_Low_Slow);
	GPIO_Init(USART1_Rx_PORT, USART1_Rx_Pin, GPIO_Mode_In_FL_No_IT);//初始化为浮空输入
	
	
	GPIO_ExternalPullUpConfig(USART1_Tx_PORT, USART1_Tx_Pin, ENABLE);
	GPIO_ExternalPullUpConfig(USART1_Rx_PORT, USART1_Rx_Pin, ENABLE);
	//GPIO_ResetBits(USART1_Tx_PORT, USART1_Tx_Pin);
	
	USART_Init(USART1, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1,
							USART_Parity_No, (USART_Mode_TypeDef)(USART_Mode_Tx|USART_Mode_Rx));
	
	//#ifdef SUPPORT_NET_FUNCTION
	#ifdef  SUPPORT_UART_DMA_FUNCTION
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	#else
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	#endif
   // #endif
	USART_Cmd(USART1, ENABLE);

	
}

#ifdef  SUPPORT_UART_DMA_FUNCTION
/******************************************************************************
**Breif:	init dma for uart																	
**Paras:                        												
**Retva:   																	
*******************************************************************************/
void DMAConfigForUart(void)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);//打开时钟，很重要	
	/* Deinitialize DMA channels */ 
	DMA_GlobalDeInit(); 
	DMA_DeInit(DMA1_Channel2);
	/* DMA channel Rx of USART Configuration */    //该函数主要要配置好接受的数组，以及USART的数据寄存器地址，数组大小，以及DMA模式
	DMA_Init(DMA1_Channel2, (uint32_t)RxBuffer, UART_DR_ADDRESS,  
	sizeof(RxBuffer)/sizeof(RxBuffer[0]),	DMA_DIR_PeripheralToMemory, DMA_Mode_Normal, 
	DMA_MemoryIncMode_Inc, DMA_Priority_High, DMA_MemoryDataSize_Byte); 

	/* Enable the USART Rx DMA requests */
	USART_DMACmd(USART1, USART_DMAReq_RX, ENABLE); 

	/* Global DMA Enable */
	DMA_GlobalCmd(ENABLE); 

	/* Enable the USART Rx DMA channel */
	DMA_Cmd(DMA1_Channel2, ENABLE); 
}


/******************************************************************************
**Description:	串口接收测试																	
** Parameters:                        												
**     Return:   																	
*******************************************************************************/
/*
void Net_receive_input(unsigned char value)
{	
	static uint8_t Flag = 0;
	uint8_t Data = 0;
	
		Data = value;		
		if(Flag == 0)
		{
			if (FRAME_FIRST==Data)
			{	
				 Flag = 1;
				 NetAO.RxFlag = RESET;
				 NetAO.RxIn = 0;
				SystemSetActiveFlag(eActiveFlagNet);
			}	
		}
		
		if(Flag == 1)
		{
			if(NetAO.RxFlag == RESET)
			{				   
				NetAO.RxBuf[NetAO.RxIn] = Data;
				//printf("RxBuf[%d] = 0x%x\n",NetAO.RxIn,Data);
				NetAO.RxIn++;
       			if (NetAO.RxIn>=RX_MAX_LEN)
				{	
					NetAO.RxFlag = SET;
					NetAO.RxIn = 0;
					Flag = 0;
					SystemSetActiveFlag(eActiveFlagNet);
				}else
				{
					;
				}
			}else
			{
				Flag = 0;
			}
		}
}
*/

void UartSendData(uint8_t data)
{
	uint16_t i = 0;//  for send timeout 16ms
	USART_SendData8(USART1, data);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
		i++;
		if(i>0xfff0)
		{
			return;
		}
	};
	
}


/******************************************************************************
**Breif:																		
**Paras:                        												
**Retva:   																	
******************************************************************************/
	/******************************************************************************
	**Description:																		
	** Parameters:																		
	**	   Return:																		
	*******************************************************************************/
	
void Net_receive_input(unsigned char value)
{
	if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(wifi_queue_buf)))
	{
		//printflog("in-out>sizeof buf\n");
	}
	else if((queue_in < queue_out) && ((queue_in	- queue_out) == 1))
	{
		//printflog("in-out==1\n");
	}	
	else
	{
			
		if(queue_in >= (unsigned char *)(wifi_queue_buf + sizeof(wifi_queue_buf)))
		{
			queue_in = (unsigned char *)(wifi_queue_buf);
		}
	
		*queue_in ++ = value;
	}
}


void UartHandleRevData(void)
{
    uint8_t i = 0;
	uint8_t Num = DMA_GetCurrDataCounter(DMA1_Channel2);
	//printf("Num = %d\n",(int)Num);  	
	for (i = 0; i <sizeof(RxBuffer)/sizeof(RxBuffer[0])-Num; ++i)//(sizeof(RxBuffer)/sizeof(RxBuffer[0]))
	{
		//printf("%x ",RxBuffer[i]);
		Net_receive_input(RxBuffer[i]);
		
	}
	//printf("\n");
	
	//to be done
	//DMA_ClearFlag(DMA_FLAG_TypeDef );
	DMA_Cmd(DMA1_Channel2, DISABLE);// CLOSE DMA
	DMA_SetCurrDataCounter(DMA1_Channel2,sizeof(RxBuffer)/sizeof(RxBuffer[0]));//the Data buffer to be reloaded.  
	DMA_Cmd(DMA1_Channel2, ENABLE);

}
#endif



#ifdef SUPPORT_DEBUG_MODE
/**********************************************************************/
//Name:			PrintfVoltage()
//Description:																  
//Parameters:                        												  
//Return:   																  
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void PrintfVoltage(uint16_t AdcValue)
{
	unsigned long Value = AdcValue;
	uint16_t Voltage = 0;

#ifdef SUPPORT_2_5V_VDD
	Value = (Value << 5) - (Value << 3) + AdcValue;				//caltmp = Hex_Val * 25; (verf=2.5V)
#elif defined(SUPPORT_2_4V_VDD)
	Value = (Value << 5) - (Value << 3) ;						//caltmp = Hex_Val * 24; (verf=2.4V)
#else
	Value = (Value << 4) +  (Value << 3) - AdcValue;			//caltmp = Hex_Val * 23; (verf=2.3V)
#endif

	Value = (Value << 3) + (Value << 1);						
	Value = (Value << 3) + (Value << 1);						

	Voltage = Value >> 12;										//Curr_Volt = caltmp / 2^n

	printf("%d.%d%d%d mV\n", Voltage/1000, Voltage/100%10, Voltage/10%10, Voltage%10);
}
#endif

