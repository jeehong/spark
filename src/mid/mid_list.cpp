#include <stdlib.h>
#include "mid_list.h"

#ifdef __cplusplus
extern "C" {
#endif

static void list_destory(struct list_t *list);
static void all_items_destory(struct list_t *list);
static void item_destory(struct list_t *list, U32 sequence);

/*
 * 
 */
struct list_t *list_init(int entity_size)
{
	struct list_t *new_list;

	new_list = (struct list_t *)malloc(sizeof(struct list_t));

	if(new_list != NULL)
	{
        new_list->destory_list = list_destory;
        new_list->destory_all_items = all_items_destory;
        new_list->destory_item = item_destory;
		new_list->entity_size = entity_size;
        new_list->ended.sequence = 0xFFFFFFFF;
		new_list->ended.data = NULL;
		new_list->ended.previous = &new_list->ended;
		new_list->ended.next = &new_list->ended;
		new_list->items = 0;
	}
	
	return new_list;
}

struct list_item_t *find_item(struct list_t *list, U32 sequence)
{
	struct list_item_t *temp_item;

	if(list == NULL)
	{
        return NULL;
	}
	for(temp_item = (struct list_item_t *)&(list->ended); 
		; 
		temp_item = temp_item->next)
	{
        if(temp_item->sequence == sequence)
		{
			return temp_item;
		}
		else if(temp_item->next == (struct list_item_t *)&(list->ended))
		{
			return NULL;
		}
	}
}

static struct list_item_t *malloc_item(U32 size)
{
    unsigned char *new_entity = NULL;
    struct list_item_t *new_item = NULL;

    new_item = (struct list_item_t *)malloc(sizeof(struct list_item_t));
    if(new_item == NULL)
    {
        return NULL;
    }
    new_entity = (unsigned char *)malloc(size);
    if(new_entity == NULL)
    {
        free(new_item);
        return NULL;
    }
    new_item->data = new_entity;

    return new_item;
}

static void free_item(struct list_item_t *item)
{
    free(item->data);
	free(item);
}

void list_insert_end(struct list_t *list, long sequence)
{
    struct list_item_t *new_item = NULL, *temp_item = &list->ended;

	
	if(list == NULL)
	{
		return;
	}

	if(find_item(list, sequence) != NULL)
	{
		return;
	}
	else
	{
        new_item = malloc_item(list->entity_size);
		if(new_item == NULL)
		{
			return;
		}
        new_item->next = temp_item;
        new_item->previous = temp_item->previous;
        temp_item->previous->next = new_item;
        temp_item->previous = new_item;
        new_item->sequence = sequence;

		list->items ++;
	}
}
	
void list_insert(struct list_t *list, U32 sequence)
{
	struct list_item_t *new_item = NULL, *temp_item;

	if(list == NULL)
	{
		return;
	}

	if(find_item(list, sequence) != NULL)
	{
		return;
	}
	else
	{
        new_item = malloc_item(list->entity_size);
		if(new_item == NULL)
		{
			return;
		}
		for( temp_item = (struct list_item_t *)&(list->ended); 
                temp_item->next->sequence <= sequence;
				temp_item = temp_item->next)
		{}
		new_item->next = temp_item->next;
		new_item->previous = temp_item;
        new_item->next->previous = new_item;
        new_item->previous->next = new_item;
        new_item->sequence = sequence;

		list->items ++;
	}	
}

static void item_destory(struct list_t *list, U32 sequence)
{
    struct list_item_t *rm_item = NULL;
	
	if(list == NULL)
	{
		return;
	}

	rm_item = find_item(list, sequence);
	if(rm_item == NULL)
	{
		return;
	}
	else
	{
		rm_item->next->previous = rm_item->previous;
		rm_item->previous->next = rm_item->next;
		free_item(rm_item);
	}
}

static void all_items_destory(struct list_t *list)
{
	struct list_item_t *temp_item;

	if(list == NULL)
	{
		return;
	}
    for(temp_item = (struct list_item_t *)(list->ended.next);
		; 
		temp_item = temp_item->next)
	{
		if(temp_item == (struct list_item_t *)&(list->ended))
		{
			return;
		}
		else
		{
			item_destory(list, temp_item->sequence);
		}
	}
}

static void list_destory(struct list_t *list)
{
	all_items_destory(list);
    free(list);
}

unsigned char *list_find_entity(struct list_t *list, U32 sequence)
{
   struct list_item_t *temp_item = NULL;

    temp_item = find_item(list, sequence);
	
    if(temp_item == NULL)
        return NULL;
    else
        return (unsigned char*)temp_item->data;
}


#ifdef __cplusplus
}
#endif


