#ifndef __MID_LIST_H__
#define __MID_LIST_H__

#include "comm_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

struct list_item_t
{
    U32 sequence;
    void *data;
	struct list_item_t *previous;
	struct list_item_t *next;
};

struct list_t
{
	struct list_item_t ended;
	int items;
	int entity_size;
    void (*destory_item)(struct list_t *, U32);
    void (*destory_all_items)(struct list_t *);
	void (*destory_list)(struct list_t *);
};

struct list_t *list_init(int entity_size);
void list_insert(struct list_t *list, U32 sequence);
unsigned char *list_find_data(struct list_t *list, U32 sequence);

#ifdef __cplusplus
}
#endif



#endif

