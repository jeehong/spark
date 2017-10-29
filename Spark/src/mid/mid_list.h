#ifndef __MID_LIST_H__
#define __MID_LIST_H__

#include "comm_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

struct list_element_t
{
	long insert_flag;
    void *data;
	struct list_element_t *next;
};

struct list_item_t
{
	struct list_element_t *head;
	int size;
	int element_size;
	void (*destory)(void *);
};

#define element_is_head(list, element)	(((struct list_item_t*)(list))->head == (struct list_element_t*)(element) ? 1 : 0)
#define element_data(element)			(((struct list_element_t*)(element))->data)
#define element_next(element)			(((struct list_element_t*)(element))->next)
#define list_head(list)					(((struct list_item_t*)(list))->head)
#define list_size(list)					(((struct list_item_t*)(list))->size)


struct list_item_t *list_init(int element_size);
void list_insert(struct list_item_t *list, long flag);
void list_remove(struct list_item_t *list, long flag);
void list_destory(struct list_item_t *list);
unsigned char *list_find_data(struct list_item_t *list, long flag);

#ifdef __cplusplus
}
#endif



#endif

