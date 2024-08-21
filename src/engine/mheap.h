#ifndef AHEAP_H
#define AHEAP_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

typedef struct {
    u32 allocated:1;
    u32 size:31;
} DataBlockHeader;

// If a target unallocated block is very close to the desired size,
// expand the block to fill it instead of splitting off a tiny chunk.
#define DEFRAG_THRESHOLD 512
#define HEAP_POOL 0x20000

extern u8 *gMemoryHeap;
extern int gMemoryHeapSize;

void *allocate(int size);
void deallocate(void *ptr);

#endif