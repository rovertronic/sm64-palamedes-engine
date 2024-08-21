#include "mheap.h"
#include "game/memory.h"

/* Arthurtilly's memory heap implementation, based off of tengoku decomp */

u8 *gMemoryHeap;
int gMemoryHeapSize = 0;

void init_memory_heap(void) {
    gMemoryHeapSize = HEAP_POOL;
    gMemoryHeap = main_pool_alloc(gMemoryHeapSize, MEMORY_POOL_LEFT);

    DataBlockHeader *header = (void *)gMemoryHeap;
    header->allocated = FALSE;
    header->size = gMemoryHeapSize;
}

void split_data_block(DataBlockHeader *header, int size) {
    DataBlockHeader *newHeader = ((void *)header) + size;
    newHeader->allocated = FALSE;
    newHeader->size = header->size - size;
    header->size = size;
}

// Allocate memory (First Fit)
void *allocate(int size) {
    size = (size + sizeof(DataBlockHeader) + 3) & ~3;
    DataBlockHeader *header = (void *)gMemoryHeap;

    while ((uintptr_t)header < (uintptr_t)gMemoryHeap + gMemoryHeapSize) {
        if (!header->allocated) {
            if (header->size >= size) {
                if (header->size > size + DEFRAG_THRESHOLD) split_data_block(header, size);
                else if (header->size > size) size = header->size; // expand slightly
                header->allocated = TRUE;
                return ((void *)header) + sizeof(DataBlockHeader);
            }
        }

        header = ((void *)header) + header->size;
    }
    return NULL;
}

void deallocate(void *ptr) {
    DataBlockHeader *header = ptr - sizeof(DataBlockHeader);
    header->allocated = FALSE;

    // Merge adjacent deallocated data blocks
    DataBlockHeader *prevHeader = (void *)gMemoryHeap;
    DataBlockHeader *currHeader = (void *)gMemoryHeap + prevHeader->size;
    while ((uintptr_t)currHeader < (uintptr_t)gMemoryHeap + gMemoryHeapSize) {
        if (!prevHeader->allocated && !currHeader->allocated) {
            prevHeader->size += currHeader->size;
        } else {
            prevHeader = currHeader;
        }
        currHeader = ((void *)currHeader) + currHeader->size;
    }
}