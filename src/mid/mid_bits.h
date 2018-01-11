#ifndef __MID_BIT_H_
#define __MID_BIT_H_

#include "src/mid/comm_typedef.h"



enum BYTES_ORDER_e
{
	CAN_FORMAT_INTEL = 0,				/*CAN INTEL*/
	CAN_FORMAT_MOTOROLA = 1			/*CAN MOTOROLA*/
};

enum BITS_ORDER_e
{
	CAN_ORDER_LSB = 0,	/*CAN ORDER LSB*/
	CAN_ORDER_MSB = 1	/*CAN ORDER MSB*/
};

#ifdef __cplusplus
extern "C" {
#endif


int data_pick(U32 *dest,
				U8* src,
				BYTES_ORDER_e bytes_order,
				BITS_ORDER_e bits_order,
				U8 start_bit,
				U8 bit_length);

int data_pack(U8* dest,
				U32 src,
				BYTES_ORDER_e bytes_order,
				BITS_ORDER_e bits_order,
				U8 start_bit,
				U8 length);

#ifdef __cplusplus
}
#endif

#endif

