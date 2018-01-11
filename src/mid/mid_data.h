#ifndef __MID_DATA_H__
#define __MID_DATA_H__

#include "comm_typedef.h"
#include "mid_bits.h"


#ifdef __cplusplus
extern "C" {
#endif

struct mid_data_config_t
{
    U32 flag;
    long id;
    BYTES_ORDER_e bytes_order;
    BITS_ORDER_e bits_order;
	U8 start_bit;
    U8 bits_length;
	U32 row;
	float factor;
	float offset;
	float phy;
};

void mid_data_can_calc(struct mid_data_config_t *dest,
                                U8 *src,
                                BYTES_ORDER_e bytes_order,
                                BITS_ORDER_e bits_order,
                                U32 src_length);


#ifdef __cplusplus
}
#endif

#endif

