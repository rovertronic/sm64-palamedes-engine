#include "linked_list.h"
#include "engine/mheap.h"

void * list_create_and_add(list * list_) {
    int item_size = list_->item_size;

    //allocate space for the data and the linked list item
    void * data = allocate(item_size);
    list_item * item = allocate(sizeof(list_item));
    
    if (data == NULL || item == NULL) {
        return NULL;
    }

    //set pointer to data in linked list item
    item->data = data;
    //set next to null since it's at the end
    item->next = NULL;

    // If empty list, set this item to be first item
    if (list_->start == NULL) {
        list_->start = item;
        list_->head = item;
        return data;
    }

    // update the old list head to point to the new list head
    list_->head->next = item;
    list_->head = item;

    return data;
}

// Same as list_create_and_add, just with existing data
void list_add(list * list_, void * data_) {
    int item_size = list_->item_size;

    //allocate space for the data
    void * data = allocate(item_size);
    //copy data into newly allocated data slot
    bcopy(data,data_,item_size);

    //allocate space for the linked list item
    list_item * item = allocate(sizeof(list_item));
    //set pointer to data in linked list item
    item->data = data;
    //set next to null since it's at the end
    item->next = NULL;

    // If empty list, set this item to be first item
    if (list_->start == NULL) {
        list_->start = item;
        list_->head = item;
        return;
    }

    // update the old list head to point to the new list head
    if (list_->head->next != NULL) {
        list_->head->next = item;
    }
    list_->head = item;
}

void list_clear(list * list_) {
    int item_size = list_->item_size;
    list_item * cur_item = list_->start;

    while (cur_item != NULL) {
        list_item * item_to_deallocate = cur_item;
        cur_item = cur_item->next;

        deallocate(item_to_deallocate->data);
        deallocate(item_to_deallocate);
    }

    list_->start = NULL;
    list_->head = NULL;
}

void list_delete(list * list_, void * data) {
    list_item * cur_item = list_->start;
    list_item * prev_item = NULL;

    while (cur_item != NULL) {

        if (cur_item->data == data) {
            //found, now delete
            if (list_->start == cur_item) {
                list_->start = cur_item->next;
                if (list_->head == cur_item) {
                    list_->head = NULL;
                }
            } else if (list_->head == cur_item) {
                list_->head = prev_item;
                prev_item->next = NULL;
            } else {
                prev_item->next = cur_item->next;
            }

            deallocate(cur_item->data);
            deallocate(cur_item);
            return;
        }

        prev_item = cur_item;
        cur_item = cur_item->next;
    }
}

s32 list_is_empty(list * list_) {
    return list_->start == NULL;
}