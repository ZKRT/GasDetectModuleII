#include "rs485.h"
#include "poison.h"
	  
//接收处理 	
volatile uint8_t  receiveValue[20];
volatile uint8_t  receiveFlag=0;
volatile uint8_t  receiveCounter=0;

/**
  * @brief  
  * @param
  * @retval 
  */
void USART1_IRQHandler(void)
{
	uint8_t res;	    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到数据
	{
	  res =USART_ReceiveData(USART1);
		receiveValue[receiveCounter++] = res;
		if(receiveCounter>=gr_cmd_handle.rec_len_bycmd)
		{
			receiveCounter = 0;
			receiveFlag = 1;
		}
	}					 
}
/**
  * @brief  rs485 初始化函数
  * @param  bound 波特率
  * @retval 
  */
void RS485_Init(uint32_t bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
	
  //串口1引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1); //GPIOA10复用为USART1
	
	//USART1    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
	//PA8推挽输出，485模式控制  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为9位数据格式,八位会不正常  
	//USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为9位数据格式,八位会不正常 //zkrt_notice
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口 1
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接受中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

	 RS485_RX_EN;				//默认为接收模式	
}


/**
  * @brief  485 发送数据
  * @param  buff 发送数组指针，len 数组长度
  * @retval 
  */
bool RS485_Send_Data(const uint8_t *buf,uint8_t len)
{
	uint8_t t;
	RS485_TX_EN		
  delay_xms (1); //	
  USART_ClearFlag(USART1,USART_FLAG_TC);
  for(t=0;t<len;t++)		
	{
      USART_SendData(USART1,buf[t]); 
      while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
	}	 		
	delay_xms (1); //
	RS485_RX_EN;
	return true;
}

/**
  * @brief  485 接收数据
  * @param  buff 接收数据数组 ，准备接收长度
  * @retval 
  */
bool RS485_Receive_Data(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	uint8_t timecnt=0;
	memset(buf, 0x00, len);
	while(timecnt <=40)
	{
		if(receiveFlag)
		{
			receiveFlag = 0;
			for(i=0; i<len; i++)
				buf[i] = receiveValue[i];
			for(i=0; i<sizeof(receiveValue); i++)
				receiveValue[i] = 0;
			return true;
		}
		delay_ms(5);
		timecnt++;
	}
	return false;
}
/**
  * @brief  阻塞方式发送后接收
  * @param  
  * @retval 
  */
bool RS485_Send_Data_Block(const uint8_t *send_data, uint8_t slen, uint8_t *recv_data, uint8_t* rlen)
{
   if(RS485_Send_Data(send_data, slen)==false)
		 return false;
	 
	 *rlen = gr_cmd_handle.rec_len_bycmd;
	 
   if(RS485_Receive_Data(recv_data, *rlen)==false)
		 return false;
	  
	 delay_ms(50);
	 return true;
}

