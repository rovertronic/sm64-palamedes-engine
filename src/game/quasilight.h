#ifndef QUASILIGHT_H
#define QUASILIGHT_H

#include <PR/ultratypes.h>

typedef struct {
    s8 x;
    s8 y;
    s8 z;
} vector_s8;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
} color_u8;

vector_s8 qsl_pl_direction(Vec3f position);
color_u8 qsl_pl_color(Vec3f position);
color_u8 qsl_ambient_color(Vec3f position);

vector_s8 qsl_sun_direction(Vec3f position);
color_u8 qsl_sun_color(Vec3f position);
void qsl_update_terrain_lighting_thread10(void);

#endif