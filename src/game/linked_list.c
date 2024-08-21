#include "linked_list.h"
#include "engine/mheap.h"

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

    // update the old list head to point to the new list head
    if (list_->head->next != NULL) {
        list_->head->next = item;
    }
    list_->head = item;
    if (list_->start == NULL) {
        list_->start = item;
    }
}