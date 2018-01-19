#ifndef __POISON_H__
#define __POISON_H__
#include "sys.h"

//length define
#define GR_MAX_CMD_LEN            7
#define GR_MAX_REC_LEN            7
//respond data len define
#define GR_MODEL_RES_LEN          7
#define GR_TYPE_RES_LEN           6
#define GR_RANGE_RES_LEN          7
#define GR_UNIT_RES_LEN           6
#define GR_SMALLPOINT_RES_LEN     6
#define GR_ERRORCODE_RES_LEN      6
#define GR_GASDATA_RES_LEN        7
//index define 
#define GR_ADDR_CMDINX            0
#define GR_CMDNUM_CMDINX          1
#define GR_LEN_CMDINX             2
#define GR_DATA_CMDINX            3
//guorui cmd
#define GRCMD_GETMOEL             0x01
#define GRCMD_GETTYPE             0x04
#define GRCMD_GETRANGE            0x05
#define GRCMD_GETUNIT             0x06
#define GRCMD_GETDECIMAL          0x07
#define GRCMD_GETERRORCODE        0x08
#define GRCMD_MODIFYADDR          0x10
#define GRCMD_GETGASDATA          0x20
#define GRCMDE_ADJUSTZERO         0x38
#define GRCMDE_ADJUSTSPAN         0x39
#define GRCMDE_ADJUSTOUTPUTZERO   0x3C
#define GRCMDE_ADJUSTOUTPUTRANGE  0x3D
//guorui address start number
#define GR_ADDR_START_NUM         0x01


enum GAS_TYPE {    
      CO=0x01, H2S, O2, Ex, N02, 
      NO, SO2, CL2, NH3, H2, 
      PH3, CH2O, O3, F2, HF, 
      HCL, HBR, C2H4O, COCL2, SIH4, 
      CLO2, CO2, SF6, TVOC, CH4
};

enum GAS_ADDRESS {
	CO_ADD=0x01, NH3_ADD, H2S_ADD, CO2_ADD, 
	TVOC_ADD, CL2_ADD, SO2_ADD, CH4_ADD
};   //add by yanly according to 8 sersor info

//send and receive gas cmd struct handle
typedef struct
{
	uint8_t cmd_data [GR_MAX_CMD_LEN];
	uint8_t rec_data [GR_MAX_REC_LEN];
	volatile uint8_t rec_len_bycmd;  //发送时已经知道，用来确定将接收多少个字节数据
}gr_gas_cmd_handle_st;

bool ReadModel (uint8_t address, uint16_t *model);//
bool ReadType (uint8_t address, uint8_t *model);
bool ReadMeaRange (uint8_t address, uint16_t *range);
bool ReadUint (uint8_t address, uint8_t *uint);
bool ReadDecimal (uint8_t address, uint8_t *decimal);
bool ReadMeaVal (uint8_t address, uint16_t *meaval);
bool CalibraZero (uint8_t address);
bool CalibraSpan (uint8_t address,uint16_t val);
bool CalibraReset(uint8_t address,uint16_t val);
bool ChangeAddr(uint8_t address,uint8_t target_addr);
extern gr_gas_cmd_handle_st gr_cmd_handle;
#endif

