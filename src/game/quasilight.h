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

typedef struct {
    Vec3f position;
    Vec3f normal;
} float_vertex;

typedef struct {
    Gfx * dl;
    void (*func)(Vtx * terrain, int size);
    struct GraphNode *node;
} dl_to_iterate;

typedef struct {
    f32 brightness;
    color_u8 color;
    Vec3f position;
    int id;
} point_light;

point_light * qsl_create_pl(Vec3f position, color_u8 color, f32 brightness);
void qsl_remove_pl(int id);

vector_s8 qsl_pl_direction(Vec3f position, point_light * pl);
color_u8 qsl_pl_color(Vec3f position, point_light * pl);

color_u8 qsl_ambient_color(Vec3f position);

vector_s8 qsl_sun_direction(Vec3f position);
color_u8 qsl_sun_color(Vec3f position);

void qsl_update_vertex_iterator_thread10(void);

Gfx *geo_object_calculate_light(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_use_point_light(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_use_plane_light(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_wiggle(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_use_global_light(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_camera_alpha(s32 callContext, struct GraphNode *node, Mat4 *mtx);

#endif