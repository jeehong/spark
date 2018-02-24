#include <math.h>
#include "mid_can.h"
#include "mid_bits.h"
#include "comm_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

const U8 can_map[64] = 
{ 
	7, 6, 5, 4, 3, 2, 1, 0,
	15, 14, 13, 12, 11, 10, 9, 8,	
	23, 22, 21, 20, 19, 18, 17, 16,
	31, 30, 29, 28, 27, 26, 25, 24,
	39, 38, 37, 36, 35, 34, 33, 32,
	47, 46, 45, 44, 43, 42, 41, 40,
	55, 54, 53, 52, 51, 50, 49, 48,
	63, 62, 61, 60, 59, 58, 57, 56
};

static int bits_pack(unsigned char *dest,
					unsigned int src,
					BYTES_ORDER_e bytes_order,
					unsigned char array_length,
					unsigned short bit_start,
					unsigned char bit_length)
{
    unsigned int old_value = 0;
    unsigned char byte_start,offset;
    int i;
	int sign_flag = 1;	/* default intel order */

    if(dest == NULL)
    {
        return ERR_POINTER_0;
    }
	
	if(bytes_order == CAN_FORMAT_MOTOROLA)
	{
		sign_flag = -1;
	}
    byte_start = bit_start >> 3;
    for(i = byte_start; (i < array_length && i >= 0) && (abs(i - byte_start) < 4); i += sign_flag)
    {
        old_value = old_value | (dest[i] << (abs(i - byte_start) << 3));
    }
    if(bit_length == 32)
    {
        old_value = 0;
        offset = 0;
    }
    else
    {
        offset = bit_start & 0x7;
        i = (1 << bit_length) - 1;
        i = ~(i << offset);
        old_value = old_value & i;
        src &= ((1 << bit_length) - 1);
    }

    i = src << offset;
    old_value = old_value | i;

    for(i = byte_start; (i < array_length && i >= 0) && (abs(i - byte_start) < 4); i += sign_flag)
    {
        dest[i] = old_value >> (abs(i - byte_start) << 3);
    }

    return STATUS_NORMAL;
}

static int bits_pick(unsigned int *dest,
                    const unsigned char *src,
                    BYTES_ORDER_e bytes_order,
                    unsigned short array_length,
                    unsigned short bit_start,
                    unsigned char bit_length)
{
    unsigned char offset;
    unsigned char byte_start;
    int i;
    unsigned int result;
	int sign_flag = 1;	/* default intel order */

    if((dest == NULL) || (src == NULL))
    {
        return ERR_POINTER_0;
    }

	if(bytes_order == CAN_FORMAT_MOTOROLA)
	{
        sign_flag = -1;
	}
    byte_start = bit_start >> 3;
    result = 0;

    for(i = byte_start; 
		(i < array_length && i >= 0) && (abs(i - byte_start) < 4); 
		i += sign_flag)
    {
        result = result | (src[i] << (abs(i-byte_start) << 3));
    }

    offset = bit_start & 0x7;
    result = result >> offset;

    if(bit_length != 32)
    {
        i = (1 << bit_length) - 1;
        result = result & i;
    }

    *dest = result;
    return STATUS_NORMAL;
}

int data_pick(U32 *dest,
					U8* src,
					BYTES_ORDER_e bytes_order,
					enum BITS_ORDER_e bits_order,
					U8 start_bit,
					U8 bit_length)
{
    if((start_bit > 63) || (src == NULL)||(dest == NULL))
    {
        return ERR_PARAMETER;
    }
    if(bits_order == CAN_ORDER_MSB)
    {
    	if(bytes_order == CAN_FORMAT_MOTOROLA)
    	{
    		start_bit = can_map[can_map[start_bit] + bit_length];
    	}
		else
		{
        	start_bit = start_bit- bit_length + 1;
		}
    }
    bits_pick(dest, src, bytes_order, MAX_DLC, start_bit, bit_length);
	
    return STATUS_NORMAL;
}

int data_pack(U8* dest,
					U32 src,
					BYTES_ORDER_e bytes_order,
					BITS_ORDER_e bits_order,
					U8 start_bit,
					U8 length)
{
    if((start_bit > 63) || (dest == NULL))
    {
        return ERR_PARAMETER;
    }

    if(bits_order == CAN_ORDER_MSB)
    {
    	if(bytes_order == CAN_FORMAT_MOTOROLA)
    	{
    		start_bit = can_map[can_map[start_bit] + length];
    	}
		else
		{
        	start_bit = start_bit-length + 1;
		}
    }

    bits_pack(dest, src, bytes_order, MAX_DLC, start_bit, length);

    return STATUS_NORMAL;
}




#ifdef __cplusplus
}
#endif

