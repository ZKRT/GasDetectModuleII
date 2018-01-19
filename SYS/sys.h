#ifndef __SYS_H__
#define __SYS_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f0xx.h"
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define makeword(a,b) (a<<8) | b

void RCC_Configuration(void);
void SysTick_Init(void);
void delay_us(uint16_t nus);				
void delay_ms(uint16_t nms);	
void delay_xms(uint16_t nms);	

extern volatile uint32_t TimingDelay;
extern volatile uint32_t led_tx_delay;
extern volatile uint32_t led_rx_delay;
extern volatile uint32_t can_hb_delay;
extern volatile uint32_t gas_read_dalay;
#endif
