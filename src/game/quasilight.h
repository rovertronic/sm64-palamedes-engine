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
    void * pointer;
} float_vertex;

typedef struct {
    Gfx * dl;
    void (*func)(Vtx * terrain, int size);
    struct GraphNode *node;
    float_vertex * addr;
} dl_to_iterate;

typedef struct {
    f32 brightness;
    color_u8 color;
    Vec3f position;
    struct Object *obj;
} point_light;

typedef struct {
    f32 brightness;
    color_u8 color;
    // -
    f32 x1;
    f32 z1;
    // +
    f32 x2;
    f32 z2;
    f32 y;
    struct Object *obj;
} plane_light;

point_light * qsl_create_pl(Vec3f position, color_u8 color, f32 brightness,  struct Object * obj);
plane_light * qsl_create_plane_light(color_u8 color, f32 brightness, f32 x1, f32 z1, f32 x2, f32 z2, f32 y, struct Object * obj);
void qsl_remove_pl(struct Object * obj);

vector_s8 qsl_pl_direction(Vec3f position, point_light * pl);
color_u8 qsl_pl_color(Vec3f position, point_light * pl);

color_u8 qsl_ambient_color(Vec3f position);

vector_s8 qsl_sun_direction(Vec3f position);
color_u8 qsl_sun_color(Vec3f position);

void qsl_update_vertex_iterator_thread10(void);
void qsl_reset(void);

Gfx *geo_terrain_use_point_light(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_wiggle(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_rope(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_use_global_light(s32 callContext, struct GraphNode *node, Mat4 *mtx);
Gfx *geo_terrain_camera_alpha(s32 callContext, struct GraphNode *node, Mat4 *mtx);

void qsl_process_object_light(Vec3f pos, struct Object *obj);

#endif