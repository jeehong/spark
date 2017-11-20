#ifndef MID_CAN_H
#define MID_CAN_H

#include "src/driver/canlib.h"

#define CAN_DEVICE_KVASER       (0)   /* Default device is Kvaser  */
#define CAN_DEVICE_USBCAN       (1)

#define CAN_SPPPORT_CHANNELS    (20)
#define CAN_CHANNEL_INFO_LENGTH (255)

#define	CAN_BAUDRATE_DEFAULT	(canBITRATE_500K)

#define MAX_DLC		(8)

#ifdef __cplusplus
extern "C" {
#endif

struct can_bus_frame_t
{
    int chn;
    long id;
    unsigned int dlc;
    unsigned int flag;
    bool new_data;
    unsigned int squ;
    unsigned char buf[MAX_DLC];
    unsigned long time_stamp;
    unsigned long delta_time_stamp;
    unsigned long config_delta_time_stamp;
};

void mid_can_refresh_device(void);
void mid_can_init(int device_type);
int mid_can_write(unsigned char *dest, unsigned char start_bit, unsigned char data_length, unsigned int data);
unsigned int mid_can_get_channels(void);
const char *mid_can_get_channel_info(unsigned int channel);
void mid_can_set_channel(unsigned int channel);
int mid_can_on_off(void);
void mid_can_set_baudrate(long baudrate);
canStatus mid_can_bus_output(bool swi);
int mid_can_apply_cfg(void);
const canBusStatistics *mid_can_process(void);
void mid_can_clear_stastic_info(void);
const struct can_bus_frame_t *mid_can_new_frame(void);
struct list_t *mid_can_tx_list(void);

#ifdef __cplusplus
}
#endif


#endif // MID_CAN_H

