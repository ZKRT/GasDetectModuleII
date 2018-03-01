#include "sys.h"

volatile uint32_t TimingDelay = 0XFFFFFFFF;
volatile uint32_t led_tx_delay = 0xffffffff;
volatile uint32_t led_rx_delay = 0xffffffff;
volatile uint32_t can_hb_delay = 0xffffffff;
volatile uint32_t gas_read_dalay = 0xffffffff;

/**
  * @brief  
  * @param  None
  * @retval None
  */
void SysTick_Init(void)
{ 
  while (SysTick_Config(SystemCoreClock / 1000) !=  0);	//配置systick，中断时间1ms
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void delay_us(uint16_t nus)				
{
	uint32_t start_num;									
	uint32_t temp;										
	uint32_t nus_pai;
	
	nus_pai = 48 * nus;
	start_num = SysTick->VAL;						
	do
	{
		temp=SysTick->VAL;								
		if (temp < start_num)						
		{
			temp = start_num - temp;				
		}
		else													
		{
			temp = start_num+48000-temp;		
		}
	}while (temp < nus_pai);      				
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void delay_ms(uint16_t nms)								
{
	uint32_t start_num;
	
	start_num = TimingDelay;								
	while ((start_num - TimingDelay) < nms);
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void delay_xms(uint16_t nms)								
{
	delay_ms(nms);
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay--;
}
//内置系统时钟配置
//void RCC_Configuration(void)
//{
//	uint32_t StartUpCounter = 0, HSIStatus = 0;
//	
//	RCC_DeInit();//RCC复位
//	RCC_HSICmd(ENABLE);//HSI使能
//	
//	while((RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET) && (StartUpCounter != HSI_STARTUP_TIMEOUT))//等待HSI使能结束
//	{
//		StartUpCounter++;
//	}
//	
//	if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)//如果使能成功了
//	{
//		HSIStatus = (uint32_t)0x01;
//	}
//	else
//	{
//		HSIStatus = (uint32_t)0x00;
//	}	
//	
//	if (HSIStatus == (uint32_t)0x01)//如果HSI使能成功
//  {
//    /* Enable Prefetch Buffer */
//		FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
//		
//		RCC_HCLKConfig(RCC_SYSCLK_Div1);//HCLK=SYSCLK
//		RCC_PCLKConfig(RCC_HCLK_Div1);  //PCLK=HCLK
//		
//		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);
//		
//		RCC_PLLCmd(ENABLE);//使能PLL时钟

//		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
//		{
//		}
//		
//		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

//		while (RCC_GetSYSCLKSource() != (uint8_t)RCC_CFGR_SWS_PLL)
//		{
//		}
//  }
//  else
//  {
//		//say something else!
//  }  
//}

float u32tof(unsigned int a)
{
	float b=*(float*)&a;
	return b;
}
unsigned int ftou32(float a)
{
	unsigned int b=*(unsigned int*)&a;
	return b;
}
