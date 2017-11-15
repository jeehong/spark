#ifndef __MID_BIT_H_
#define __MID_BIT_H_

#include "src/mid/comm_typedef.h"

#define CAN_FORMAT_INTEL 0				/*CAN INTEL*/
#define CAN_FORMAT_MOTOROLA 1			/*CAN MOTOROLA*/

#define CAN_ORDER_LSB 0					/*CAN ORDER LSB*/
#define CAN_ORDER_MSB 1                 /*CAN ORDER MSB*/

#ifdef __cplusplus
extern "C" {
#endif


int data_pick(U32 *dest,
				U8* src,
				U8 bytes_order,
				U8 bits_order,
				U8 start_bit,
				U8 bit_length);

int data_pack(U8* dest,
				U32 src,
				U8 bytes_order,
				U8 bits_order,
				U8 start_bit,
				U8 length);

#ifdef __cplusplus
}
#endif

#endif

