#ifndef GENERAL_BATCHER_H
#define GENERAL_BATCHER_H

#include <PR/ultratypes.h>
#include "types.h"

typedef struct batchable batchable;
struct batchable {
    batchable * next;
    Vec3f * pos;
    Mat4 * transform;
};

void general_batcher_batch_pos(Gfx * material, Gfx * shape, batchable * start);
void general_batcher_batch_mtx(Gfx * material, Gfx * shape, batchable * start);

#endif