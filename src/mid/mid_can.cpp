#include <stdlib.h>
#include "src/driver/canlib.h"
#include "mid_can.h"
#include "mid_bits.h"
#include "mid_list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mid_can_running_t
{
    int handle;
    int channels;
    int connect;
    char chn_info[CAN_SPPPORT_CHANNELS][CAN_CHANNEL_INFO_LENGTH];
	char get_chns_err[CAN_CHANNEL_INFO_LENGTH];
	struct list_t *rx_list;
	struct list_t *tx_list;
    int cur_chn;
    unsigned int baudrate;
	int bus_on;
	canBusStatistics status;
};

static struct mid_can_running_t kvaser;

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
    canSetBusParams(kvaser.handle, kvaser.baudrate, 0, 0, 0, 0, 0);
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

struct list_t *mid_can_tx_list(void)
{
    return kvaser.tx_list;
}

static void filling_data(void)
{
    canStatus stat;
    struct can_bus_frame_t src, *dest;

	while(1)
	{
        stat = canRead(kvaser.handle,
                        &src.id, &src.buf[0],
                        &src.dlc,
                        &src.flag,
                        &src.time_stamp);
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
            dest = (struct can_bus_frame_t *)list_find_entity(kvaser.rx_list, src.id);
            src.chn = kvaser.cur_chn;
            src.new_data = TRUE;
            if(src.time_stamp > dest->time_stamp)
                src.delta_time_stamp = src.time_stamp - dest->time_stamp;
            else
                src.delta_time_stamp = dest->time_stamp - src.time_stamp;
            memcpy(dest, &src, sizeof(*dest));

		}
	}
}

static void tx_process(void)
{
    struct can_bus_frame_t *temp_frame = NULL;
    struct list_item_t *temp_element;
    unsigned long time_now;

	
    temp_element = kvaser.tx_list->ended.next;
    time_now = canReadTimer(kvaser.handle);
    while(temp_element != &kvaser.tx_list->ended)
    {
        temp_frame = (struct can_bus_frame_t*)(temp_element->data);
        if(abs(time_now - temp_frame->time_stamp)
                >= temp_frame->config_delta_time_stamp)
        {
            temp_frame->delta_time_stamp = abs(time_now - temp_frame->time_stamp);
            canWrite(kvaser.handle, temp_frame->id, temp_frame->buf, temp_frame->dlc, temp_frame->flag);
            temp_frame->time_stamp = time_now;
        }
        temp_element = temp_element->next;
    }
}

const canBusStatistics *mid_can_process(void)
{
	canStatus stat;
    tx_process();
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
    struct can_bus_frame_t *temp_frame = NULL;
    struct list_item_t *temp_element;

    if(kvaser.rx_list->ended.next == &kvaser.rx_list->ended)
    {
        return NULL;
    }
    temp_element = kvaser.rx_list->ended.next;
    while(temp_element != &kvaser.rx_list->ended)
    {
        if(((struct can_bus_frame_t*)temp_element->data)->new_data == TRUE)
        {
            temp_frame = (struct can_bus_frame_t*)(temp_element->data);
            temp_frame->new_data = FALSE;
            break;
        }
        temp_element = temp_element->next;
    }

    return temp_frame;
}

#if 0
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
#endif

#ifdef __cplusplus
}

#endif
