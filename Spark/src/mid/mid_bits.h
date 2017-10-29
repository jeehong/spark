#ifndef __MID_BIT_H_
#define __MID_BIT_H_

#include "src/mid/comm_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

int bits_pack(unsigned int src,
                    unsigned char *array,
                    unsigned char array_length,
                    unsigned short bit_start,
                    unsigned char bit_length);

int bits_parse(unsigned int *dest,
                    const unsigned char *array,
                    unsigned short array_length,
                    unsigned short bit_start,
                    unsigned char bit_length);

#ifdef __cplusplus
}
#endif

#endif

