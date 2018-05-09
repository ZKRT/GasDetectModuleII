/**
  ******************************************************************************
  * @file    guorui.c
  * @author  ZKRT
  * @version V1.0
  * @date    5-January-2018
  * @brief
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
#include "guorui.h"
#include "poison.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
gr_ch_infost gr_ch_info[MAX_CHANNEL_GAS];
gr_dev_infost gr_dev_info;

/**
  * @brief
  * @param  None
  * @retval None
  */
void guorui_init(void) {
	int i;
	uint16_t tempshort;
	uint8_t tempchar;

	//clear global variables
	memset(gr_ch_info, 0x00, sizeof(gr_ch_info));
	memset(&gr_dev_info, 0x00, sizeof(gr_dev_info));

	//read ch number and config ch map
	for (i = GR_ADDR_START_NUM; i <= MAX_CHANNEL_GAS; i++) {
		delay_ms(100);
		if (ReadModel(i, &tempshort) == true) {
			gr_ch_info[gr_dev_info.ch_num].addr = i;
			gr_ch_info[gr_dev_info.ch_num].model = tempshort;
			gr_dev_info.ch_num++;
		}
	}
	//read all valid channel number info like range, decimal so on and so forth, except gas value
	for (i = 0; i < gr_dev_info.ch_num; i++) { //read type
		delay_ms(100);
		if (ReadType(gr_ch_info[i].addr, &tempchar) == true) {
			gr_ch_info[i].type = tempchar;
		}
	}
	for (i = 0; i < gr_dev_info.ch_num; i++) { //read range
		delay_ms(100);
		if (ReadMeaRange(gr_ch_info[i].addr, &tempshort) == true) {
			gr_ch_info[i].range = tempshort;
		}
	}
	for (i = 0; i < gr_dev_info.ch_num; i++) { //read unit
		delay_ms(100);
		if (ReadUint(gr_ch_info[i].addr, &tempchar) == true) {
			gr_ch_info[i].unit = tempchar;
		}
	}
	for (i = 0; i < gr_dev_info.ch_num; i++) { //read decimal
		delay_ms(100);
		if (ReadDecimal(gr_ch_info[i].addr, &tempchar) == true) {
			gr_ch_info[i].decimal = tempchar;
		}
	}
}
/**
  * @brief
  * @param  None
  * @retval None
  */
void guorui_prcs(void) {
	static int get_gas_data_cnt = 0;
	uint16_t tempshort;
	if (gas_read_dalay - TimingDelay >= 500) { //every 500ms period read one gas value, gas order is "get_gas_data_cnt"
		gas_read_dalay = TimingDelay;
		if (ReadMeaVal(gr_ch_info[get_gas_data_cnt].addr, &tempshort) == true) {
			gr_ch_info[get_gas_data_cnt].gasvalue = tempshort;
			gr_ch_info[get_gas_data_cnt].status = 1;
			gr_dev_info.status |=  (1U << (get_gas_data_cnt));
		} else {
			gr_ch_info[get_gas_data_cnt].gasvalue = 0;
			gr_ch_info[get_gas_data_cnt].status = 0;
			gr_dev_info.status &= ~(1U << (get_gas_data_cnt));
		}
		get_gas_data_cnt++;
		if (get_gas_data_cnt >= gr_dev_info.ch_num)
			get_gas_data_cnt = 0;
	}
}

/* Private functions ---------------------------------------------------------*/
