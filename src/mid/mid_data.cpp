#include "mid_data.h"
#include "mid_bits.h"

#ifdef __cplusplus
extern "C" {
#endif

void mid_data_can_calc(struct mid_data_config_t *dest, 
								U8 *src, 
								bool bytes_order, 
								bool bits_order, 
								U32 src_length)
{
    if(dest == NULL)
    {
        return;
    }
    if(data_pick(&dest->row,
					src,
					bytes_order,
					bits_order,
					dest->start_bit,
					dest->bits_length) < 0)
    {
        dest->row = 0;
        dest->phy = 0;
        return;
    }
    dest->phy = dest->row * dest->factor + dest->offset;
}

#ifdef __cplusplus
}
#endif



