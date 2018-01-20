#include "usart.h"
#include "rs485.h"
#include "poison.h"
/**
  ******************************************************************************
  * @file    usart.c 
  * @author  ZKRT
  * @version V1.0
  * @date    13-December-2016
  * @brief   串口配置
	*					 + (1) init
	*                       
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  * @param  None
  * @retval None
  */
void usart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1); 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接受中断

	//Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
}

///////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  //加入以下代码,支持printf函数,而不需要选择use MicroLIB
  */
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x)
{ 
	x = x;
}
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
		;
	USART_SendData(USART2, (unsigned char) ch);
  return (ch);
}
#endif

/**
  * @brief  
  * @param
  * @retval 
  */

void USART2_IRQHandler(void)
{
	uint8_t res;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{
	  res =USART_ReceiveData(USART2);
		if(sendflag ==1)
			return;
		send485[sendindex] = res;
		if(sendindex==0)
		{
			senlennn =0;
		}
		if(sendindex ==1)
		{
			switch(send485[sendindex])
			{
				case 0x01:
					senlennn =5;
					break;
				case 0x04:
					senlennn =5;
					break;
				case 0x05:
					senlennn =5;
					break;				
				case 0x06:
					senlennn =5;
					break;
				case 0x07:
					senlennn =5;
					break;	
				case 0x10:
					senlennn =6;
					break;	
				case 0x20:
					senlennn =5;
					break;	
				case 0x2e:
					senlennn =6;
					break;	
				case 0x2f:
					senlennn =6;
					break;	
				case 0x38:
					senlennn =5;
					break;	
				case 0x39:
					senlennn =7;
				case 0x3a:
					senlennn =11;
				case 0x3b:
					senlennn =13;
				case 0x3c:
					senlennn =7;
				case 0x3d:
					senlennn =7;
				default:
					sendindex = 0;
				  senlennn = 0;
					break;
			}	
		}
		
		sendindex++;
		if((sendindex ==senlennn)&&(sendindex>0)&&(senlennn>0))
		{
			sendflag = 1;
		}
		
//		RS485_TX_EN;
////		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
////			;
//		USART_SendData(USART1, (unsigned char) res);
//		gas_read_dalay = TimingDelay;
	}					 
}

///////////////////////////////////////////////////////////////////////////////////

/**
*@
*/
