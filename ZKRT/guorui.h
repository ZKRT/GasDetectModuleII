/**
  ******************************************************************************
  * @file    guorui.h 
  * @author  ZKRT
  * @version V1.0
  * @date    5-January-2018
  * @brief   
	*					 + (1) init
  ******************************************************************************
  * @attention
  *
  * ...
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GUORUI_H
#define __GUORUI_H 
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "appprotocol.h"
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
//guorui gas type
typedef enum
{
	none_grst=0,
	co_grst=1,
	h2s_grst,
	o2_grst,
	ex_grst,
	no2_grst,
	no_grst,
	so2_grst,
	cl2_grst,
	nh3_grst,
	h2_grst,
	ph3_grst,
	ch2o_grst,
	o3_grst,
	f2_grst,
	hf_grst,
	hcl_grst,
	hbr_grst,
	c2h4o_grst,
	cocl2_grst,
	sih4_grst,
	clo2_grst,
	co2_grst,
	sf6_grst,
	voc_grst,
	ch4_grst,
	max_grst
}gr_gas_sensortype;

//guorui unit type
typedef enum
{
	PPM_grunit=0,
	PPB_grunit,
	PPM2_grunit,
	permillage_grunit,
	percent_grunit,	
	LELpercent_grunit,
	VOLpercent_grunit,
	mgdiviedm3_gruint,
	mgdiviedL_grunit,
	max_grunit
}gr_gas_unite;

//气体地址与通道映射等关系
typedef struct
{
	uint8_t ch_num; //总有效通道数,实时读取
	uint16_t status; //通道是否正常位标记，一共可标记16个通道。
}gr_dev_infost;

typedef struct{
	uint8_t addr;
	uint8_t status;
  uint16_t model;
  uint8_t type;
  uint16_t range;
  uint8_t unit;
  uint8_t decimal;
	uint16_t gasvalue;
//	uint32_t gasvaluefloat;
//	uint32_t rangefloat;
}gr_ch_infost;

/* Exported functions ------------------------------------------------------- */
void guorui_init(void);
void guorui_prcs(void);

extern gr_ch_infost gr_ch_info[MAX_CHANNEL_GAS];
extern gr_dev_infost gr_dev_info;
#endif /* __GUOGUI_H */
/**
  * @}
  */ 

/**
  * @}
  */
	
/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

