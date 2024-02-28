#ifndef GENERAL_BATCHER_H
#define GENERAL_BATCHER_H

#include <PR/ultratypes.h>
#include "types.h"

typedef struct batchable batchable;
struct batchable {
    batchable * next;
    Mat4 * transform;
    Vec3f * pos;
};

#endif