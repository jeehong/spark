#include <stdlib.h>
#include "mid_list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct list_item_t *list_init(int element_size)
{
	struct list_item_t *new_item;

	new_item = (struct list_item_t *)malloc(sizeof(struct list_item_t));

	if(new_item != NULL)
	{
		new_item->destory = NULL;
		new_item->element_size = element_size;
		new_item->head = NULL;
		new_item->size = 0;
	}
	
	return new_item;
}

static struct list_element_t *find_pre_element(struct list_item_t *list, long flag)
{
	struct list_element_t *temp;
	
	if(list == NULL)
        return NULL;
	temp = list->head;
	while(temp->next != NULL)
	{
		if(flag == temp->next->insert_flag)
		{	
            return temp;
		}
		temp = temp->next;
	}

    return NULL;
}


static struct list_element_t *find_element(struct list_item_t *list, long flag)
{
    struct list_element_t *temp;

    if(list == NULL)
        return NULL;
    if(list->head == NULL)
        return NULL;
    temp = list->head;
    while(temp->next != NULL)
    {
        if(flag == temp->insert_flag)
        {
            return temp;
        }
        temp = temp->next;
    }
    if(temp->insert_flag == flag)
        return temp;
    else
        return NULL;
}

void list_insert(struct list_item_t *list, long flag)
{
    struct list_element_t *new_element, *temp;

	if(list == NULL)
        return;
	if(list->head != NULL && find_element(list, flag) != NULL)
        return;
	new_element = (struct list_element_t *)malloc(sizeof(struct list_element_t));
	if(new_element == NULL)
        return;
	new_element->data = malloc(list->element_size);
	if(new_element->data == NULL)
	{
		free(new_element);
		return;
	}
	new_element->insert_flag = flag;
	new_element->next = NULL;
	
	if(list->head == NULL)
	{
		list->head = new_element;
	}
	else
	{
        temp = list->head;
        while(temp->next != NULL)
		{
            if(new_element->insert_flag < temp->insert_flag)
            {
				break;
            }
			temp = temp->next;
        }
        if(temp == list->head
                && new_element->insert_flag < temp->insert_flag)
        {
            new_element->next = list->head;
            list->head = new_element;
        }
        else
        {
            new_element->next = temp->next;
            temp->next = new_element;
        }
    }
}

void list_remove(struct list_item_t *list, long flag)
{
	struct list_element_t *temp, *rm_obj;
	
	if(list == NULL)
		return;

	temp = find_pre_element(list, flag);
	if(temp != NULL)
	{
		rm_obj = temp->next;
		temp->next = temp->next->next;
		free(rm_obj->data);
		free(rm_obj);
	}
}

unsigned char *list_find_data(struct list_item_t *list, long flag)
{
    struct list_element_t *element = find_element(list, flag);

    if(element == NULL)
        return NULL;
    else
        return (unsigned char*)element->data;
}

void list_destory(struct list_item_t *list)
{
}


#ifdef __cplusplus
}
#endif


