#include"adc.h"

#define  ADCNUM      (1<<2)


/**********************************************************************/
//Description:	AdcConfig()								
//Parameters:                        									
//Return:   															
//Date:           
//Author:		quanwu.xu  
/**********************************************************************/
void AdcConfig(ADC_SamplingTime_TypeDef ADC_SamplingTime)
{
	CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
	ADC_DeInit(ADC1); 
	ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_1);
	ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime);
	ADC_Cmd(ADC1, ENABLE);
}


/*****************************************************************************
*函数名称 : AdcGetChannelValue
*功能描述 : ADC配置以及通道转换后的数据获取
*输入参数 :     ADC_SamplingTime：分频
		  ：ADC_Channels		：通道
*返回参数 :
*使用说明 : 
*****************************************************************************/

uint16_t AdcGetChannelValue(ADC_SamplingTime_TypeDef ADC_SamplingTime, ADC_Channel_TypeDef ADC_Channels)
{ 
    uint8_t i = 0;
    uint16_t AdcValue=0;
    
    AdcConfig(ADC_SamplingTime);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ChannelCmd(ADC1, ADC_Channels, ENABLE); 
	Delay(5);
	
    for(i=0;i<ADCNUM;i++)
    { 
    	
		#ifdef SUPPORT_TIMING_DEBUG
    		GPIO_SetBits(TIMING_DEBUG_PORT, TIMING_DEBUG_PIN);
		#endif	  
        /* Start ADC1 Conversion using Software trigger*/
        ADC_SoftwareStartConv(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        ADC_ClearFlag(ADC1, ADC_FLAG_EOC);		
		#ifdef SUPPORT_TIMING_DEBUG
        	GPIO_ResetBits(TIMING_DEBUG_PORT, TIMING_DEBUG_PIN);
		#endif		
	    AdcValue += ADC_GetConversionValue (ADC1);	

    }
    ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd(ADC1, ADC_Channels, DISABLE);
    AdcValue >>= 2;

    return AdcValue;
}


/**
  * @brief   ADC value to mv
  * @param  None
  * @retval None
  */
uint16_t AdcConvertTomVoltage(uint16_t AdcValue)
{
    uint16_t CurrVolt  = 0;
    uint32_t CalTmp = 0;
//----------------------------------------
    CalTmp = AdcValue;
    CalTmp = (CalTmp << 5) - (CalTmp << 3) + CalTmp;            //caltmp = Hex_Val * 25; (verf=2.5V)

    CalTmp = (CalTmp << 3) + (CalTmp << 1);                     //caltmp = caltmp * 10
    CalTmp = (CalTmp << 3) + (CalTmp << 1);                     //caltmp = caltmp * 10
    CurrVolt = CalTmp >> 12;                                   //Curr_Volt = caltmp / 2^n   

    return CurrVolt;
}

float MVoltageConvertToVoltage(uint16_t CurrVolt)
{  
    return (float)CurrVolt/1000;
}



