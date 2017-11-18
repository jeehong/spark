#include <stdlib.h>
#include "mid_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _mid_pool_t
{
	U8 index;
	U32 avg;
	U32 sum;
	Bool complete;
	U8 num;
	U32 *pdata;
} mid_pool_t;

static struct _mid_pool_t mid_pool[MID_POOLS_NUM_MAX];


ERROR_CODE mid_pool_register(U8 num)
{
	static U8 id;

	if(id >= MID_POOLS_NUM_MAX)
		return ERR_FULL;

    mid_pool[id].pdata = (U32 *)malloc(num * sizeof(mid_pool[0].pdata[0]));

	if(mid_pool[id].pdata == NULL)
		return ERR_POINTER_0;
	
	mid_pool[id].num = num;
	mid_pool_var_reset(id); 
	id ++;
	
	return id - 1;
}

ERROR_CODE mid_pool_var_reset(U8 id)
{
	if(id >= MID_POOLS_NUM_MAX)
		return ERR_FULL;

	mid_pool[id].avg = 0;
	mid_pool[id].sum = 0;
	mid_pool[id].index = 0;
	mid_pool[id].complete = FALSE;
	memset(mid_pool[id].pdata, 0, mid_pool[id].num * sizeof(mid_pool[id].pdata[0]));
	
	return STATUS_NORMAL;
}

ERROR_CODE mid_pool_push_var(U8 id, U32 var, U8 num)
{
	U8 i;

	if(id >= MID_POOLS_NUM_MAX)
		return ERR_FULL;

	if(var > 0xFFFFFF)
		return ERR_PARAMETER;

	for (i = 0; i < num; i++)
	{
		mid_pool[id].sum -= mid_pool[id].pdata[mid_pool[id].index];
		mid_pool[id].pdata[mid_pool[id].index] = var;
		mid_pool[id].sum += var;

		mid_pool[id].index ++;
		if(mid_pool[id].index >= mid_pool[id].num)
			mid_pool[id].complete = TRUE;		
		mid_pool[id].index %= mid_pool[id].num;
	}
	return STATUS_NORMAL;
}

U32 mid_pool_get_avg(U8 id)
{
	if(id >= MID_POOLS_NUM_MAX)
		return 0;
	
	if(mid_pool[id].complete == TRUE)
		mid_pool[id].avg = mid_pool[id].sum / mid_pool[id].num;
	else
	{
		if(mid_pool[id].index)
			mid_pool[id].avg = mid_pool[id].sum / mid_pool[id].index;
		else
			mid_pool[id].avg = 0;
	}

	return mid_pool[id].avg;
}

inline Bool mid_pool_get_complete(U8 id)
{
	if(id >= MID_POOLS_NUM_MAX)
		return 0;

	return mid_pool[id].complete;
}

#ifdef __cplusplus
}
#endif

