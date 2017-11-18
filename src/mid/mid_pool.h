#ifndef __MID_POOL_H__
#define __MID_POOL_H__

#include "comm_typedef.h"

#define MID_POOLS_NUM_MAX		10

#ifdef __cplusplus
extern "C" {
#endif

ERROR_CODE mid_pool_register(U8 num);

ERROR_CODE mid_pool_push_var(U8 id, U32 var, U8 num);

ERROR_CODE mid_pool_var_reset(U8 id);

U32 mid_pool_get_avg(U8 id);

Bool mid_pool_get_complete(U8 id);

#ifdef __cplusplus
}
#endif


#endif
