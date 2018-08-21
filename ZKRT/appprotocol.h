/**
  ******************************************************************************
  * @file    appprotocol.h
  * @author  ZKRT
  * @version V1.0
  * @date    9-May-2017
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
#ifndef __APPPROTOCOL_H
#define __APPPROTOCOL_H
/* Includes ------------------------------------------------------------------*/
#include "zkrt.h"
/* Exported macro ------------------------------------------------------------*/
//version and model string //zkrt_notice
#define DEV_MODEL              "M-GD"
#define DEV_HW                 "000101"
#define DEV_SW                 "000102"

/* Exported constants --------------------------------------------------------*/
//////////////////////////////////////////////////////special define
//gas
#define MAX_CHANNEL_GAS              8

//type num enum
typedef enum {
	TN_None = 0,
	TN_HEARTBEAT,
	TN_GETDEVINFO,
	TN_MAX
} TypeNumEnum;
//hb flag
#define TNHB_FLAG                     0xaabbccdd
#define THHB_FIXED_LEN                5
/* Exported types ------------------------------------------------------------*/
#pragma pack(1)
///////////////////////////////subdev to uav zkrt data struct
//////common command
typedef struct {
	u8 type_num;
	u8 type_data[ZK_DATA_MAX_LEN - 1];
} common_data_plst;
//heartbeat data
typedef struct {
	u32 hb_flag;
	u8 hb_data[ZK_DATA_MAX_LEN - THHB_FIXED_LEN];
} common_hbd_plst;
//gas heartbeat
typedef struct {
	u8 ch_num;
	u16 ch_status;
	u32 gas_value[MAX_CHANNEL_GAS];  //max channel num is 8
} hb_gas_st;
//dev info data
typedef struct {
	u8 status;
	u8 model[10];
	u8 hw_version[6];
	u8 sw_version[6];
} common_get_devinfo_plst;
//////special command
//respond flag
#define NEED_RETRUN                   1
#define NOTNEED_RETRUN                0

#define RES_HEADER_LEN  2  //equal to control num+status
//type num enum
typedef enum {
	NONE_GASCN = 0,
	GETCHNUM_GASCN = 1,
	GETCHINFO_GASCN,
	MAX_GASCN
} gas_ctrlnume;
//gas respond status enum
typedef enum {
	S_Success_Gas,
	S_Fail_Gas,
	S_FailNoDev_Gas,
	S_FailNoChannel_Gas
} ResStatusEnumGas;
typedef struct {
	u8 control_num;
	u8 other_data[ZK_DATA_MAX_LEN - 1];
} send_comspec_plst;
typedef struct {
	u8 control_num;
	u8 status;
	u8 other_data[ZK_DATA_MAX_LEN - 2];
} respond_comspec_plst;
//////send and respond sub
////device type=gas type
//get gas channel num  --not send, have respond
typedef struct {
	u8 ch_num;
} rgetchnum_gas_comspecplst;
//get gas channel info
typedef struct {
	u8 ch;
} getchinfo_gas_comspecplst;
typedef struct {
	u8 ch;
	u8 reseved[2];
	u32 value; //32bit float
	u8 reseved2[2];
	u16 sensor_type;
	u16 unit_type;
	u32 range;  //32bit float
	u8 reserved3;
} rgetchinfo_gas_comspecplst;
///////////////////////////////
#pragma pack()
/* Exported functions ------------------------------------------------------- */
#endif /* __APPCAN_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT ZKRT *****END OF FILE****/

