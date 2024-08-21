#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "engine/mheap.h"

struct list_item {
    void * data;
    struct list_item * next;
};

struct list {
    int item_size;
    struct list_item * start;
    struct list_item * head;
};

typedef struct list list;
typedef struct list_item list_item;

void * list_create_and_add(list * list_);
void list_add(list * list_, void * data_);
void list_clear(list * list_);
s32 list_is_empty(list * list_);
void list_delete(list * list_, void * data);


#endif