#ifndef __RS485_H
#define __RS485_H
#include "sys.h"

//模式控制
//#define RS485_TX_EN		PAout(8)	//485模式控制.0,接收;1,发送.
#define RS485_TX_EN		 GPIO_SetBits(GPIOA,GPIO_Pin_8);//485模式控制.0,接收;1,发送.
#define RS485_RX_EN		 GPIO_ResetBits(GPIOA,GPIO_Pin_8);//485模式控制.0,接收;1,发送.

void RS485_Init(uint32_t bound);
bool RS485_Send_Data(const uint8_t *buf, uint8_t len);
bool RS485_Receive_Data(uint8_t *buf, uint8_t len);
bool RS485_Send_Data_Block(const uint8_t *send_data, uint8_t slen, uint8_t *recv_data, uint8_t* rlen);
#endif
















