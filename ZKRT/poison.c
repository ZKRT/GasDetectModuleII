#include "poison.h"
#include "rs485.h"

gr_gas_cmd_handle_st gr_cmd_handle;
uint8_t *cmd_data = gr_cmd_handle.cmd_data;
uint8_t *rec_data = gr_cmd_handle.rec_data;
volatile uint8_t *reclen_bycmd = &gr_cmd_handle.rec_len_bycmd;

/**
  * @brief  ����У����
  * @param  pSendBuf ����ָ�� ��nEnd ���鳤��
  * @retval ����16λУ��ֵ
  */
unsigned int GetCheck (unsigned char *pSendBuf, unsigned char nEnd ) {
	unsigned char i, j;
	unsigned int wCrc = (unsigned int)(0xffff);
	for (i = 0; i < nEnd; i++) {
		wCrc ^= (unsigned int)(pSendBuf[i]);
		for (j = 0; j < 8; j++ ) {
			if (wCrc & 0x01) {
				wCrc >>= 1;
				wCrc ^= 0xA001;
			} else wCrc >>= 1;
		}
	}
	return wCrc;
}
/**
  * @brief  ��ȡ�ͺ�
  * @param  address �ӻ���ַ,model �ͺ�ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
bool ReadModel(uint8_t address, uint16_t *model) {
	uint16_t check = 0;
	uint8_t i = 0, j = 0;
	cmd_data[i++] = address;
	cmd_data[i++] = GRCMD_GETMOEL;
	cmd_data[i++] = 0X00;
	check = GetCheck(cmd_data, 3);
	cmd_data[i++] = check & 0XFF;
	cmd_data[i++] = (check >> 8) & 0XFF;

	*reclen_bycmd = GR_MODEL_RES_LEN;

	if (RS485_Send_Data_Block(cmd_data, i, rec_data, &j) == false)
		return false;

	check = GetCheck(rec_data, j - 2);
	if ((check & 0xff) == rec_data[j - 2] && ((check >> 8) & 0XFF) == rec_data[j - 1]) {
		memcpy(model, rec_data + GR_DATA_CMDINX, 2);
		return true;
	} else
		return false;
}

/**
  * @brief  ��ȡ��������
  * @param  address �ӻ���ַ,type ����ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
bool ReadType (uint8_t address, uint8_t *type) {
	uint16_t check = 0;
	uint8_t i = 0, j = 0;
	cmd_data[i++] = address;
	cmd_data[i++] = GRCMD_GETTYPE;
	cmd_data[i++] = 0X00;
	check = GetCheck(cmd_data, 3);
	cmd_data[i++] = check & 0XFF;
	cmd_data[i++] = (check >> 8) & 0XFF;

	*reclen_bycmd = GR_TYPE_RES_LEN;

	if (RS485_Send_Data_Block(cmd_data, i, rec_data, &j) == false)
		return false;

	check = GetCheck(rec_data, j - 2);
	if ((check & 0xff) == rec_data[4] && ((check >> 8) & 0XFF) == rec_data[5]) {
		*type = rec_data[3];
		return true;
	} else return false;
}

/**
  * @brief  ��ȡ����
  * @param  address �ӻ���ַ,range ������ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
bool ReadMeaRange (uint8_t address, uint16_t *range) {
	uint16_t check = 0;
	uint8_t i = 0, j = 0;
	cmd_data[i++] = address;
	cmd_data[i++] = GRCMD_GETRANGE;
	cmd_data[i++] = 0X00;
	check = GetCheck(cmd_data, 3);
	cmd_data[i++] = check & 0XFF;
	cmd_data[i++] = (check >> 8) & 0XFF;
	*reclen_bycmd = GR_RANGE_RES_LEN;

	if (RS485_Send_Data_Block(cmd_data, i, rec_data, &j) == false)
		return false;

	check = GetCheck(rec_data, 5);
	if ((check & 0xff) == rec_data[5] && ((check >> 8) & 0XFF) == rec_data[6]) {
		*range = makeword(rec_data[4], rec_data[3]);
		return true;
	} else
		return false;
}
/**
  * @brief  ��ȡ��λ
  * @param  address �ӻ���ַ,uint ��λ��ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
bool ReadUint (uint8_t address, uint8_t *uint) {
	uint16_t check = 0;
	uint8_t i = 0, j = 0;
	cmd_data[i++] = address;   //��ַ
	cmd_data[i++] = 0X06;     //����
	cmd_data[i++] = 0X00;     //���ݳ���
	check = GetCheck(cmd_data, 3);
	cmd_data[i++] = check & 0XFF;     //У���λ
	cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
	*reclen_bycmd = GR_UNIT_RES_LEN;

	if (RS485_Send_Data_Block(cmd_data, i, rec_data, &j) == false)
		return false;
	check = GetCheck(rec_data, 4);
	if ((check & 0xff) == rec_data[4] && ((check >> 8) & 0XFF) == rec_data[5]) {
		*uint = rec_data[3];
		return true;
	} else return false;
}

/**
  * @brief  ��ȡС��
  * @param  address �ӻ���ַ,decimal С����ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
bool ReadDecimal (uint8_t address, uint8_t *decimal) {
	uint16_t check = 0;
	uint8_t i = 0, j = 0;
	cmd_data[i++] = address;   //��ַ
	cmd_data[i++] = 0X07;     //����
	cmd_data[i++] = 0X00;     //���ݳ���
	check = GetCheck(cmd_data, 3);
	cmd_data[i++] = check & 0XFF;     //У���λ
	cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
	*reclen_bycmd = GR_SMALLPOINT_RES_LEN;

	if (RS485_Send_Data_Block(cmd_data, i, rec_data, &j) == false)
		return false;
	check = GetCheck(rec_data, 4);
	if ((check & 0xff) == rec_data[4] && ((check >> 8) & 0XFF) == rec_data[5]) {
		*decimal = rec_data[3];
		return true;
	} else return false;
}

/**
  * @brief  ��ȡ����ֵ
  * @param  address �ӻ���ַ,value������ֵ
  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
  */
bool ReadMeaVal (uint8_t address, uint16_t *meaval) {
	uint16_t check = 0;
	uint8_t i = 0, j = 0;
	cmd_data[i++] = address;   //��ַ
	cmd_data[i++] = 0X20;     //����
	cmd_data[i++] = 0X00;     //���ݳ���
	check = GetCheck(cmd_data, 3);
	cmd_data[i++] = check & 0XFF;     //У���λ
	cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
	*reclen_bycmd = GR_GASDATA_RES_LEN;

	if (RS485_Send_Data_Block(cmd_data, i, rec_data, &j) == false)
		return false;

	check = GetCheck(rec_data, 5);
	if ((check & 0xff) == rec_data[5] && ((check >> 8) & 0XFF) == rec_data[6]) {
		*meaval = makeword(rec_data[4], rec_data[3]);
		return true;
	} else return false;
}

///**
//  * @brief  У��zero
//  * @param  address �ӻ���ַ
//  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
//  */
//bool CalibraZero (uint8_t address)
//{
//   uint16_t check = 0;
//   uint8_t i = 0;
//   cmd_data[i++] = address;   //��ַ
//   cmd_data[i++] = 0X38;     //����
//   cmd_data[i++] = 0X00;     //���ݳ���
//   check = GetCheck(cmd_data, 3);
//   cmd_data[i++] = check & 0XFF;     //У���λ
//   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
//   RS485_Send_Data(cmd_data,i);
//
//   RS485_Receive_Data(rec_data, 5);
//   check = GetCheck(rec_data, 3);
//   if((check&0xff) == rec_data[3] && ((check>>8)&0XFF) ==rec_data[4]) return TRUE;
//   else return FALSE;
//}

///**
//  * @brief  У��span
//  * @param  address �ӻ���ַ
//  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
//  */
//bool CalibraSpan (uint8_t address,uint16_t val)
//{
//   uint16_t check = 0;
//   uint8_t i = 0;
//   cmd_data[i++] = address;   //��ַ
//   cmd_data[i++] = 0X39;     //����
//   cmd_data[i++] = 0X02;     //���ݳ���
//   cmd_data[i++] = val & 0XFF;
//   cmd_data[i++] = (val>>8) & 0XFF;
//   check = GetCheck(cmd_data, 5);
//   cmd_data[i++] = check & 0XFF;     //У���λ
//   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
//   RS485_Send_Data(cmd_data,i);
//
//   RS485_Receive_Data(rec_data, 5);
//   check = GetCheck(rec_data, 3);
//   if((check&0xff) == rec_data[3] && ((check>>8)&0XFF) ==rec_data[4]) return TRUE;
//   else return FALSE;
//}

///**
//  * @brief  У���궨
//  * @param  address �ӻ���ַ���궨��ֵ
//  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
//  */
//bool CalibraReset(uint8_t address,uint16_t val)
//{
//   uint16_t check = 0;
//   uint8_t i = 0;
//   cmd_data[i++] = address;   //��ַ
//   cmd_data[i++] = 0X3E;     //����
//   cmd_data[i++] = 0X02;     //���ݳ���
//   cmd_data[i++] = val & 0XFF;
//   cmd_data[i++] = (val>>8) & 0XFF;
//   check = GetCheck(cmd_data, 5);
//   cmd_data[i++] = check & 0XFF;     //У���λ
//   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
//   RS485_Send_Data(cmd_data,i);
//
//   RS485_Receive_Data(rec_data, 6);
//   check = GetCheck(rec_data, 4);
//   if((check&0xff) == rec_data[4] && ((check>>8)&0XFF) ==rec_data[5]) return TRUE;
//   else return FALSE;
//}

///**
//  * @brief  �޸Ĵ�������ַ
//  * @param  address ԭ�ӻ���ַ��target_addrĿ���ַ
//  * @retval ��ȡ�ɹ�����TRUE ʧ�ܷ��� FALSE
//  */
//bool ChangeAddr(uint8_t address,uint8_t target_addr)
//{
//   uint16_t check = 0;
//   uint8_t i = 0;
//   cmd_data[i++] = address;   //��ַ
//   cmd_data[i++] = 0X10;     //����
//   cmd_data[i++] = 0X01;     //���ݳ���
//   cmd_data[i++] = target_addr;
//   check = GetCheck(cmd_data, 4);
//   cmd_data[i++] = check & 0XFF;     //У���λ
//   cmd_data[i++] = (check >> 8) & 0XFF;     //У���λ
//   RS485_Send_Data(cmd_data,i);
//
//   RS485_Receive_Data(rec_data, 5);
//   check = GetCheck(rec_data, 3);
//   if((check&0xff) == rec_data[3] && ((check>>8)&0XFF) ==rec_data[4]) return TRUE;
//   else return FALSE;
//}
