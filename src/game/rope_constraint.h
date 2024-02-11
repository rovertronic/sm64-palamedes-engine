#ifndef ROPE_CONSTRAINT_H
#define ROPE_CONSTRAINT_H

#include <PR/ultratypes.h>
#include "types.h"

#define ROPE_SURFACE_POOL_SIZE 200

typedef struct rope_joint rope_joint;
struct rope_joint {
    rope_joint * next;
    rope_joint * prev;
    f32 y;
    f32 lerp_y;
    f32 y_vel;
    u8 pinned;
    Vec3f world_position;
};

typedef struct {
    struct Surface * surf;
    struct Surface home_surf;
    struct rope_joint * vertexs_joint[3];
} rope_surface;

extern rope_surface rope_surface_pool[ROPE_SURFACE_POOL_SIZE];
extern int rope_surface_count;

extern void rope_step(void);
extern rope_joint * rope_nearest_joint(Vec3f position);

#endif