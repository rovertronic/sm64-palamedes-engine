#include "engine/math_util.h"
#include "game_init.h"
#include "quasilight.h"
#include "memory.h"
#include "level_update.h"
#include "area.h"

#include "levels/test/header.h"

/*
QUASILIGHT.C 
Quasilight is a lighting engine for Super Mario 64.
The core concept is that objects use directional lights to simulate lighting while
the environment uses vertex colors to simulate terrain lighting. Multi-threading is used
to ensure that environment lighting does not lag general gameplay.
*/

#define CLAMP_0(x) ((x < 0) ? 0 : x)
#define CLAMP_255(x) ((x > 255) ? 255 : x)

float_vertex qsl_vertex_pool[10000];
int qsl_vertex_index = 0;

Vec3f qsl_global_sun_direction = {0.0f, 1.0f, 0.0f};
color_u8 qsl_global_sun_color = {0,0,0};//{90/2, 95/2, 100/2};
color_u8 qsl_global_ambient_color = {0,0,0};//{50/2, 45/2, 50/2};

Vec3f qsl_pl_source = {0.0f, 0.0f, 0.0f};

vector_s8 vec3f_to_vector_s8(Vec3f vector) {
    vector_s8 returnvec;
    returnvec.x = vector[0] * 126.0f;
    returnvec.y = vector[1] * 126.0f;
    returnvec.z = vector[2] * 126.0f;

    return returnvec;
}

vector_s8 qsl_pl_direction(Vec3f position) {
    Vec3f source = {0.0f, 0.0f, 0.0f};
    vec3f_copy(source,qsl_pl_source);

    Vec3f light_direction;
    vec3f_diff(light_direction,source,position);
    vec3f_normalize(light_direction);

    return vec3f_to_vector_s8(light_direction);
}

color_u8 qsl_pl_color(Vec3f position) {
    Vec3f source = {0.0f, 0.0f, 0.0f};
    vec3f_copy(source,qsl_pl_source);

    Vec3f transformed_light;
    vec3f_diff(transformed_light,source,position);

    f32 dist = vec3_mag(transformed_light);
    f32 brightness = 8.0f;
    color_u8 light_here;
    light_here.r = CLAMP_0(255 - (dist/brightness));
    light_here.g = CLAMP_0(255 - (dist/brightness));
    light_here.b = CLAMP_0(255 - (dist/brightness))*0;
    return light_here;
}

color_u8 qsl_ambient_color(Vec3f position) {
    //color_u8 ambient_here = {10,10,10};

    return qsl_global_ambient_color;
}

color_u8 qsl_sun_color(Vec3f position) {
    //color_u8 sun_here = {80,80,80};
    return qsl_global_sun_color;
}

vector_s8 qsl_sun_direction(Vec3f position) {
    //Vec3f sun_direction = {0.0f, -1.0f, 0.0f};
    //vec3f_normalize(sun_direction);

    return vec3f_to_vector_s8(qsl_global_sun_direction);
}

color_u8 qsl_color_env(Vec3f position, Vec3f point_normal) {
    f32 sun_coverage = (1.0f + vec3f_dot(point_normal,qsl_global_sun_direction))/2.0f;
    color_u8 sun_color = qsl_sun_color(position);

    color_u8 ambient = qsl_ambient_color(position);

    Vec3f source;
    f32 brightness = 8.0f;
    color_u8 point_col = qsl_pl_color(position);
    vec3f_copy(source,qsl_pl_source);
    Vec3f transformed_light;
    vec3f_diff(transformed_light,source,position);
    f32 dist = vec3_mag(transformed_light);
    Vec3f light_direction;
    vec3f_diff(light_direction,source,position);
    vec3f_normalize(light_direction);
    f32 surf_coverage = (1.0f + vec3f_dot(point_normal,light_direction))/2.0f;

    color_u8 light_here;
    light_here.r = CLAMP_255(ambient.r + (sun_coverage*sun_color.r) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.r/127.0f) );
    light_here.g = CLAMP_255(ambient.g + (sun_coverage*sun_color.g) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.g/127.0f) );
    light_here.b = CLAMP_255(ambient.b + (sun_coverage*sun_color.b) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.b/127.0f) );
    return light_here;
}

u8 penis_debug = 0;

void qsl_init_vtx_list(Vtx * terrain, int size) {
    for (int i = 0; i < size; i++) {
        qsl_vertex_pool[qsl_vertex_index].normal[0] = terrain[i].n.n[0];
        qsl_vertex_pool[qsl_vertex_index].normal[1] = terrain[i].n.n[1];
        qsl_vertex_pool[qsl_vertex_index].normal[2] = terrain[i].n.n[2];
        vec3f_normalize(qsl_vertex_pool[qsl_vertex_index].normal);

        qsl_vertex_pool[qsl_vertex_index].position[0] = terrain[i].v.ob[0];
        qsl_vertex_pool[qsl_vertex_index].position[1] = terrain[i].v.ob[1];
        qsl_vertex_pool[qsl_vertex_index].position[2] = terrain[i].v.ob[2];
        qsl_vertex_index ++;
    }
};

void qsl_update_vtx_list(Vtx * terrain, int size) {
    for (int i = 0; i < size; i++) {
        color_u8 color = qsl_color_env(qsl_vertex_pool[qsl_vertex_index].position, qsl_vertex_pool[qsl_vertex_index].normal);
        terrain[i].v.cn[0] = color.r;
        terrain[i].v.cn[1] = color.g;
        terrain[i].v.cn[2] = color.b;

        qsl_vertex_index++;
    }
}

u32 * qsl_gfx_stack[40];

void qsl_update_terrain_lighting_thread10(void) {
    //qsl_init_terrain_lighting();

    u32 * command_read = segmented_to_virtual(test_dl_bob_mesh_layer_1);
    int qsl_gfx_stack_level = 0;
    u8 end_of_list = FALSE;
    qsl_vertex_index = 0;

    while(!end_of_list) {
        if  ( ((*command_read)>>24) == G_DL)  {
            qsl_gfx_stack[qsl_gfx_stack_level] = command_read+2;
            qsl_gfx_stack_level++;
            command_read = segmented_to_virtual(*(command_read+1));
        }
        if ( ((*command_read)>>24) == G_VTX) {
            int size = (((*command_read)>>12) & 0xFF);

            qsl_init_vtx_list( segmented_to_virtual(*(command_read+1)), size);
        }
        if ( ((*command_read)>>24) == G_ENDDL) {
            if (qsl_gfx_stack_level == 0) {
                end_of_list = TRUE;
            } else {
                qsl_gfx_stack_level--;
                command_read = qsl_gfx_stack[qsl_gfx_stack_level];
            }
        } else {
            command_read+=2;
        }
    }

    while (TRUE) {

        //if (gMarioState->controller->buttonDown & L_TRIG) {
            vec3f_copy(qsl_pl_source,gMarioState->pos);
        //}

        command_read = segmented_to_virtual(test_dl_bob_mesh_layer_1);
        end_of_list = FALSE;
        qsl_gfx_stack_level = 0;
        qsl_vertex_index = 0;

        while(!end_of_list) {
            if  ( ((*command_read)>>24) == G_DL)  {
                qsl_gfx_stack[qsl_gfx_stack_level] = command_read+2;
                qsl_gfx_stack_level++;

                command_read = segmented_to_virtual(*(command_read+1));
            }
            if ( ((*command_read)>>24) == G_VTX) {
                int size = (((*command_read)>>12) & 0xFF);
                qsl_update_vtx_list( segmented_to_virtual(*(command_read+1)), size);
            }
            if ( ((*command_read)>>24) == G_ENDDL) {
                if (qsl_gfx_stack_level == 0) {
                    end_of_list = TRUE;
                } else {
                    qsl_gfx_stack_level--;
                    command_read = qsl_gfx_stack[qsl_gfx_stack_level];
                }
            } else {
                command_read+=2;
            }
        }
    }
}