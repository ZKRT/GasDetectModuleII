#include "led.h" 

/**
  * @brief  
  * @param  None
  * @retval None
  */
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;   //pin6 can tx, pin7 can rx, pin5 alarm, pin4 run
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void led_prcs(void)
{
	if(led_rx_delay - TimingDelay> 50)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_7);
	}
	if(led_tx_delay - TimingDelay> 50)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
	}
}
