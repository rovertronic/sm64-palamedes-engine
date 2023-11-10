#ifndef QUASILIGHT_H
#define QUASILIGHT_H

#include <PR/ultratypes.h>

typedef struct {
    s8 x;
    s8 y;
    s8 z;
} vector_s8;

typedef struct {
    s8 r;
    s8 g;
    s8 b;
} color_s8;

vector_s8 qsl_nearest_pl_direction(Vec3f position);
color_s8 qsl_nearest_pl_color(Vec3f position);

#endif