#include "sys.h"
#include "rs485.h"
#include "can.h"
#include "led.h"
#include "usart.h"
#include "appgas.h"
#include "guorui.h"

/**
  * @brief  
  * @param  None
  * @retval None
  */
int main(void )
{
	SysTick_Init();
	RS485_Init(9600);
	usart_init();
	while (1)
	{
		if(sendflag)
		{
			RS485_Send_Data((void*)send485, senlennn);
			sendflag =0;
			sendindex =0;
		}
	}
}
