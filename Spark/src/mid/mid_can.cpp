#include <stdlib.h>
#include "src/driver/canlib.h"
#include "mid_can.h"
#include "mid_bits.h"
#include "src/app/spark.h"
#include "mid_list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct kvaser_running_t
{
    int handle;
    int channels;
    int connect;
    char chn_info[CAN_SPPPORT_CHANNELS][CAN_CHANNEL_INFO_LENGTH];
	char get_chns_err[CAN_CHANNEL_INFO_LENGTH];
	struct list_item_t *rx_list;
	struct list_item_t *tx_list;
    int cur_chn;
    unsigned int baudrate;
	int bus_on;
	canBusStatistics status;
};

static struct kvaser_running_t kvaser;

void mid_can_refresh_device(void)
{
	canStatus stat;
    int channels;

    kvaser.channels = 0;
    stat = canGetNumberOfChannels(&channels);
    if(canOK == stat)
    {
        int index;
        char temp[CAN_CHANNEL_INFO_LENGTH];
        for(index = 0; index < channels; index ++)
        {
            stat = canGetChannelData(index, canCHANNELDATA_DEVDESCR_ASCII, temp, CAN_CHANNEL_INFO_LENGTH);
            if(stat < 0)
            {
                canGetErrorText(stat, &kvaser.get_chns_err[0], CAN_CHANNEL_INFO_LENGTH);
            }
            else
            {
                memcpy(&kvaser.chn_info[kvaser.channels][0], temp, CAN_CHANNEL_INFO_LENGTH);
                kvaser.channels ++;
            }
        }
    }
    else
    {
        canGetErrorText(stat, &kvaser.get_chns_err[0], CAN_CHANNEL_INFO_LENGTH);
    }		
}

void mid_can_set_channel(unsigned int channel)
{
    if(channel >= CAN_SPPPORT_CHANNELS)
        return;
    kvaser.cur_chn = channel;

}

void mid_can_set_baudrate(unsigned int baudrate)
{
    kvaser.baudrate = baudrate;
}

void mid_can_apply_cfg(void)
{
    kvaser.handle = canOpenChannel(kvaser.cur_chn, canOPEN_ACCEPT_VIRTUAL);
    
    kvFlashLeds(kvaser.handle, kvLED_ACTION_ALL_LEDS_ON, 2000);
}

int mid_can_on_off(void)
{
    if(kvaser.connect == FALSE)
    {
		kvaser.handle = canOpenChannel(kvaser.cur_chn, canOPEN_ACCEPT_VIRTUAL);
		canSetBusParams(kvaser.handle, kvaser.baudrate, 0, 0, 0, 0, 0);
        canBusOn(kvaser.handle);
        kvaser.bus_on = TRUE;
        kvaser.connect = TRUE;
    }
    else
    {
        canBusOff(kvaser.handle);
        canClose(kvaser.handle);
        kvaser.bus_on = FALSE;
        kvaser.connect = FALSE;
    }

    return kvaser.connect;
}

unsigned int mid_can_get_channels(void)
{
	return kvaser.channels;
}

const char *mid_can_get_channel_info(unsigned int channel)
{
	if(channel >= CAN_SPPPORT_CHANNELS)
		return NULL;

	return &kvaser.chn_info[channel][0];
}

void mid_can_clear_stastic_info(void)
{
    memset(&kvaser.status, 0, sizeof(kvaser.status));
}

void mid_can_init(int device_type)
{
	kvaser.rx_list = list_init(sizeof(struct can_bus_frame_t));
	kvaser.tx_list = list_init(sizeof(struct can_bus_frame_t));
	kvaser.baudrate = CAN_BAUDRATE_DEFAULT;
    if(device_type == CAN_DEVICE_KVASER)
    {
        canInitializeLibrary();
    }
	mid_can_refresh_device();
}

static void filling_data(void)
{
    canStatus stat;
    struct can_bus_frame_t src, *dest;

	while(1)
	{
		stat = canRead(kvaser.handle, &src.id, &src.buf[0], &src.dlc, &src.flag, &src.time_stamp);
		if (stat < 0)
		{
			break;
		}
		if(src.flag & canMSG_ERROR_FRAME)
		{
		}
		else
		{
			list_insert(kvaser.rx_list, src.id);
            dest = (struct can_bus_frame_t *)list_find_data(kvaser.rx_list, src.id);
            src.chn = kvaser.cur_chn;
			src.new_data = TRUE;
			memcpy(dest, &src, sizeof(*dest));
		}
	}
}

const canBusStatistics *mid_can_process(void)
{
	canStatus stat;
	stat = canRequestBusStatistics(kvaser.handle);
    if(stat < 0)
    {
		
    }else
    {
        stat = canGetBusStatistics(kvaser.handle, &kvaser.status, sizeof(canBusStatistics));
	}

	filling_data();
    return &kvaser.status;
}

const struct can_bus_frame_t *mid_can_new_frame(void)
{
    struct can_bus_frame_t *new_frame = NULL;
    struct list_element_t *new_element;

    if(kvaser.rx_list->head == NULL)
        return NULL;
    new_element = kvaser.rx_list->head;
    do
    {
        if(((struct can_bus_frame_t*)new_element->data)->new_data == TRUE)
        {
            new_frame = (struct can_bus_frame_t*)(new_element->data);
            new_frame->new_data = FALSE;
            break;
        }
        new_element = new_element->next;
    }while(new_element != NULL && new_element->next != NULL);

    return new_frame;
}

int mid_can_write(unsigned char *dest,
                  unsigned char start_bit,
                  unsigned char data_length,
                  unsigned int data)
{
	
}

int mid_can_read(unsigned char *dest,
                  unsigned char start_bit,
                  unsigned char data_length,
                  unsigned int data)
{
	
}

#ifdef __cplusplus
}

#endif
