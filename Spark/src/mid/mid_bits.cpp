#include "mid_bits.h"

#ifdef __cplusplus
extern "C" {
#endif


int bits_pack(unsigned int src,
					unsigned char *array,
					unsigned char array_length,
					unsigned short bit_start,
					unsigned char bit_length)
{
	unsigned int old_value = 0;
	unsigned char byte_start,offset; 
	unsigned int i;

	if(array == NULL)
	{
		return ERR_POINTER_0;
	}

	byte_start = bit_start >> 3; 
	for(i = byte_start; (i < array_length) && (i - byte_start < 4); i++)
	{
		old_value = old_value | (array[i] << ((i - byte_start) << 3));
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
	
	
	for(i = byte_start; (i < array_length) && (i - byte_start < 4); i++)
	{
		array[i] = old_value >> ((i - byte_start) << 3);
	}
	
	return STATUS_NORMAL;
}

int bits_parse(unsigned int *dest,
					const unsigned char *array, 
					unsigned short array_length, 
					unsigned short bit_start, 
					unsigned char bit_length)
{
	unsigned char offset;
	unsigned char byte_start;
	unsigned int i;
	unsigned int result;

	if((dest == NULL) || (array == NULL))
	{
		return ERR_POINTER_0;
	}
	
	byte_start = bit_start >> 3;
	result = 0;
	
	for(i = byte_start; (i < array_length) && (i - byte_start < 4); i++)
	{
		result = result | (array[i] << ((i-byte_start) << 3));
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


#ifdef __cplusplus
}
#endif

