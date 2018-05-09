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
int main(void ) {
	SysTick_Init();
	RS485_Init(9600);
	usart_init();
	CAN_Mode_Init(CAN_Mode_Normal);
	LED_Init();
	delay_ms(15000);//�ϵ�һ��ʱ��֮�󣬲���ȥ���������Ƿ���ڣ���Ȼ������ʴ˴����һ����ʱ  //zkrt_debug
	guorui_init();
	while (1) {
		led_prcs();
		guorui_prcs();
		appcan_prcs();
	}
}
