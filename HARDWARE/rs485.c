#include "rs485.h"
#include "poison.h"
	  
//���մ��� 	
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
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//���յ�����
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
  * @brief  rs485 ��ʼ������
  * @param  bound ������
  * @retval 
  */
void RS485_Init(uint32_t bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
	
  //����1���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1); //GPIOA10����ΪUSART1
	
	//USART1    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
	//PA8���������485ģʽ����  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ9λ���ݸ�ʽ,��λ�᲻����  
	//USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ9λ���ݸ�ʽ,��λ�᲻���� //zkrt_notice
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ��� 1
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

	 RS485_RX_EN;				//Ĭ��Ϊ����ģʽ	
}


/**
  * @brief  485 ��������
  * @param  buff ��������ָ�룬len ���鳤��
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
  * @brief  485 ��������
  * @param  buff ������������ ��׼�����ճ���
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
  * @brief  ������ʽ���ͺ����
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

