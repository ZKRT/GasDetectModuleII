/**
  ******************************************************************************
  * @file    appgas.c 
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
#include "appgas.h"
#include "guorui.h"
#include "zkrt_gas.h"
#include "appprotocol.h"
#include "can.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define MANU_NUM  1
//gas type map array, zkrt gas type mapp to guorui gas type
const u8 gas_type_map[max_zkst][MANU_NUM] = {
	{0xff}, {co_grst}, {h2s_grst}, {nh3_grst}, {co2_grst}, {cl2_grst}, {so2_grst}, {voc_grst}, {ch4_grst}, {o2_grst}
};
//gas unit type map array -- guorui
const u8 gas_unit_map[max_zkunit][MANU_NUM] = {
	{0xff}, {VOLpercent_grunit}, {LELpercent_grunit}, {PPM_grunit}, {0xff}, {percent_grunit}, {0xff}, {PPB_grunit}, {permillage_grunit}, {mgdiviedm3_gruint}, {mgdiviedL_grunit}
};

recv_zkrt_packet_handlest recv_handle={0};
uint8_t can_send_data[ZK_MAX_LEN];
uint8_t can_send_len;
/* Private functions ---------------------------------------------------------*/
static void can_recv_handle(void);
static void can_hb_handle(void);
static u8 zkrt_decode(recv_zkrt_packet_handlest *rh);
static u8 appgas_spec_message_handle(zkrt_packet_t* recvpack, u8* respond_data, u8* reslen);
//protocol fun
static char none_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char getchnum_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen);
static char getchinfo_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen);
typedef char (*GASPTOCOL_FUN)(void*, void*, u8, u8*);
const GASPTOCOL_FUN gasptcol_fun[] = {
	none_gasptf,
	getchnum_gasptf,
	getchinfo_gasptf
};
/**
  * @brief  
  * @param  None
  * @retval None
  */
void appcan_prcs(void)
{
	can_recv_handle();
	can_hb_handle();
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void can_hb_handle(void)
{
	int i;
	zkrt_packet_t *packet = (zkrt_packet_t*)can_send_data;
	common_data_plst *comn_data = (common_data_plst*)packet->data;
	common_hbd_plst *comnhb = (common_hbd_plst*)comn_data->type_data;
	hb_gas_st *hbgas = (hb_gas_st*)comnhb->hb_data;	
	if(can_hb_delay -TimingDelay <1000)
		return;
	can_hb_delay = TimingDelay;
	//updata
	//init packet
	zkrt_init_packet(packet);
	//header
	packet->cmd = SUBDEV_TO_UAV;
	packet->command = ZK_COMMAND_COMMON;
	packet->UAVID[ZK_DINDEX_DEVTYPE] = DEVICE_TYPE_SELF;
	//common data
	comn_data->type_num = TN_HEARTBEAT;
	//hb data
	comnhb->hb_flag = TNHB_FLAG;
	//gas data
	hbgas->ch_num = gr_dev_info.ch_num;
	hbgas->ch_status = gr_dev_info.status;
	for(i=0; i<hbgas->ch_num; i++)
	{
		float value;
		uint16_t devidev;
		if(gr_ch_info[i].decimal ==0)
			devidev = 1;
		else
			devidev = (gr_ch_info[i].decimal)*10;
		value = (float)(gr_ch_info[i].gasvalue)/devidev;
		memcpy(&hbgas->gas_value[i], &value, 4);		
	}
	packet->length = THHB_FIXED_LEN+sizeof(hb_gas_st);
	can_send_len = zkrt_final_encode(can_send_data, packet);
	//send
	CAN1_send_message_fun(can_send_data, can_send_len);
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static void can_recv_handle(void)
{
	if(zkrt_decode(&recv_handle) ==0)
		return;
	
	switch (recv_handle.packet.command)
	{
		case ZK_COMMAND_COMMON:
			break;		
		case ZK_COMMAND_SPECIFIED:
			if(appgas_spec_message_handle(&recv_handle.packet, can_send_data, &can_send_len)==NEED_RETRUN)
			{
				CAN1_send_message_fun(can_send_data, can_send_len);
			}
			break;
		case ZK_COMMAND_NORMAL:          
			break;
		default:break;
	}	
}
/**
  * @brief   
  * @param  None
  * @retval None
  */
static u8 zkrt_decode(recv_zkrt_packet_handlest *rh)
{
	uint8_t can_value;
	while(CAN1_rx_check() == 1)
	{
		can_value = CAN1_rx_byte();
		if (zkrt_decode_char_v2(rh, can_value)==1)
		{
			return 1;
		}
	}
	return 0;
}
/**
  * @brief  
  * @param  None
  * @retval None
  */
static u8 appgas_spec_message_handle(zkrt_packet_t* recvpack, u8* respond_data, u8* reslen)
{
	u8 ret =NEED_RETRUN;
	u8 respacket_len;
	send_comspec_plst *comspec = (send_comspec_plst*)recvpack->data;
	respond_comspec_plst *rcomspec = (respond_comspec_plst*)(respond_data+sizeof(zkrtpacket_header));
	zkrtpacket_header* resheader = (zkrtpacket_header*)respond_data;
	zkrtpacket_tailer* restailer;
	
	if(recvpack->command != ZK_COMMAND_SPECIFIED)
		return NOTNEED_RETRUN;
	
	if((comspec->control_num <=NONE_GASCN)||(comspec->control_num >=MAX_GASCN))
		return NOTNEED_RETRUN;
	
	//parse and pack respond packet
	ret = gasptcol_fun[recvpack->data[0]](recvpack->data, rcomspec, recvpack->length, &respacket_len);
	//pack respond_data header and tailer and so on so forth
	if(ret==NEED_RETRUN)
	{
		memcpy(resheader, recvpack, sizeof(zkrtpacket_header));
		resheader->cmd = UAV_TO_APP;
		resheader->length = respacket_len;
		restailer = (zkrtpacket_tailer*)(rcomspec+respacket_len);
		restailer->end_code = _END_CODE;
		restailer->crc = crc_calculate(respond_data, respacket_len+ZK_HEADER_LEN);
		*reslen = respacket_len+ZK_FIXED_LEN;
	}
	return ret;
}
/////////////////////////////////////////////////////////////////// zkrt gas type special 
u8 manu_gastype2zkrt(u8 manu, u8 type)
{
	int i;
	if((manu >=MANU_NUM)||(type==0xff))
		return 0xff;
	for(i=0; i< max_zkst; i++)
	{
		if(gas_type_map[i][manu] == type)
			return i;
	}
	return 0xff;
}
u8 manu_gasunittype2zkrt(u8 manu, u8 type)
{
	int i;
	if((manu >MANU_NUM)||(type==0xff))
		return 0xff;
	for(i=0; i< max_zkst; i++)
	{
		if(gas_unit_map[i][manu] == type)
			return i;
	}
	return 0xff;	
}
///////////////////////////////////////////////////////////////////////////////app gas protocl funciton
static char none_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	return NOTNEED_RETRUN;
}
//
static char getchnum_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res = S_Success_Gas;
	send_comspec_plst *s = (send_comspec_plst*)sdata;
	respond_comspec_plst *r = (respond_comspec_plst*)rdata;
	rgetchnum_gas_comspecplst *rother = (rgetchnum_gas_comspecplst*)r->other_data;
	
	if(gr_dev_info.ch_num ==0)
	{
		res = S_FailNoDev_Gas;
	}
	
	rother->ch_num = gr_dev_info.ch_num;
	
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rgetchnum_gas_comspecplst) + RES_HEADER_LEN;
	
	return NEED_RETRUN;	
}
//
static char getchinfo_gasptf(void *sdata, void *rdata, u8 slen, u8* rlen)
{
	u8 res = S_Success_Gas;
	send_comspec_plst *s = (send_comspec_plst*)sdata;
	respond_comspec_plst *r = (respond_comspec_plst*)rdata;
	getchinfo_gas_comspecplst *sother = (getchinfo_gas_comspecplst*)s->other_data;
	rgetchinfo_gas_comspecplst *rother = (rgetchinfo_gas_comspecplst*)r->other_data;
	
	if(gr_dev_info.ch_num ==0)
		res = S_FailNoDev_Gas;
	
	if(sother->ch >= gr_dev_info.ch_num)
		res = S_FailNoChannel_Gas;
	
	if(res==S_Success_Gas)
	{
		if(sother->ch <gr_dev_info.ch_num)
		{
			float value;
		  uint16_t devidev;
      if(gr_ch_info[sother->ch].decimal ==0)
				devidev = 1;
			else
				devidev = (gr_ch_info[sother->ch].decimal)*10;
			value = (float)(gr_ch_info[sother->ch].gasvalue)/devidev;
			memcpy(&rother->value, &value, 4);
			rother->ch = sother->ch;
			rother->sensor_type = manu_gastype2zkrt(0, gr_ch_info[sother->ch].type);
			rother->unit_type = manu_gasunittype2zkrt(0, gr_ch_info[sother->ch].unit);
			value = (float)gr_ch_info[sother->ch].range;
			memcpy(&rother->range, &value, 4);
		}
		else
			res = S_FailNoChannel_Gas; 
	}
	//respond header
	r->control_num = s->control_num;
	r->status = res;
	*rlen = sizeof(rgetchinfo_gas_comspecplst) + RES_HEADER_LEN;
	
	return NEED_RETRUN;	
}

