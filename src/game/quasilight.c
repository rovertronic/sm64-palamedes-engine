#include "main.h"
#include "engine/math_util.h"
#include "engine/mheap.h"
#include "game_init.h"
#include "quasilight.h"
#include "memory.h"
#include "level_update.h"
#include "area.h"
#include "camera.h"
#include "buffers/buffers.h"
#include "audio/external.h"
#include "rope_constraint.h"

#include "levels/test/header.h"

/*
QUASILIGHT.C 
Quasilight is a vertex-iteration system & lighting engine for Super Mario 64.
The core concept is that objects use directional lights to simulate lighting while
the environment uses vertex colors to simulate terrain lighting. Multi-threading is used
to ensure that vertex-iteration does not lag general gameplay. You plug in a display list
and you can apply lighting, have it become transparent when between mario and the camera,
and have a vertex wiggle animation.
*/

#define CLAMP_0(x) ((x < 0) ? 0 : x)
#define CLAMP_255(x) ((x > 255) ? 255 : x)

int qsl_vertex_index = 0;

point_light qsl_point_light_pool[40];
plane_light qsl_plane_light_pool[10];
point_light * qsl_second_nearest_pl = NULL;
int qsl_point_light_count = 0;
int qsl_plane_light_count = 0;

dl_to_iterate qsl_dl_pool[20];
dl_to_iterate * curr_qsl_dl;
int qsl_dl_count = 0;
u8 qsl_dl_flagged = FALSE;

Vec3f qsl_global_sun_direction = {0.0f, 1.0f, 0.0f};
color_u8 qsl_global_sun_color = {40,49,50};//{90/2, 95/2, 100/2};
color_u8 qsl_global_ambient_color = {10,10,30};//{50/2, 45/2, 50/2};

vector_s8 vec3f_to_vector_s8(Vec3f vector) {
    vector_s8 returnvec;
    returnvec.x = vector[0] * 126.0f;
    returnvec.y = vector[1] * 126.0f;
    returnvec.z = vector[2] * 126.0f;

    return returnvec;
}

point_light * qsl_create_pl(Vec3f position, color_u8 color, f32 brightness, struct Object * obj) {
    vec3f_copy(qsl_point_light_pool[qsl_point_light_count].position,position);
    qsl_point_light_pool[qsl_point_light_count].color = color;
    qsl_point_light_pool[qsl_point_light_count].brightness = brightness;
    qsl_point_light_pool[qsl_point_light_count].obj = obj;
    qsl_point_light_count++;
    return &qsl_point_light_pool[qsl_point_light_count-1];
}

plane_light * qsl_create_plane_light(color_u8 color, f32 brightness, f32 x1, f32 z1, f32 x2, f32 z2, f32 y, struct Object * obj) {
    qsl_plane_light_pool[qsl_plane_light_count].x1 = x1;
    qsl_plane_light_pool[qsl_plane_light_count].z1 = z1;
    qsl_plane_light_pool[qsl_plane_light_count].x2 = x2;
    qsl_plane_light_pool[qsl_plane_light_count].z2 = z2;
    qsl_plane_light_pool[qsl_plane_light_count].y = y;

    qsl_plane_light_pool[qsl_plane_light_count].color = color;
    qsl_plane_light_pool[qsl_plane_light_count].brightness = brightness;
    qsl_plane_light_pool[qsl_plane_light_count].obj = obj;
    qsl_plane_light_count++;
    return &qsl_plane_light_pool[qsl_plane_light_count-1];
}

void qsl_remove_pl(struct Object * obj) {
    for (int i=0; i<qsl_point_light_count; i++) {
        if (qsl_point_light_pool[i].obj == obj) {
            for (int j=i; j<qsl_point_light_count; j++) {
                bcopy(&qsl_point_light_pool[j+1],&qsl_point_light_pool[j],sizeof(qsl_point_light_pool[0]));
                if (qsl_point_light_pool[j].obj) {
                    qsl_point_light_pool[j].obj->pl = &qsl_point_light_pool[j];
                }
            }
            qsl_point_light_count--;
            return;
        }
    }
}

point_light * qsl_pl_nearest(Vec3f position) {
    f32 smallest_dist = 99999.0f;
    point_light * nearest_pl = NULL;

    for(int i=0; i<qsl_point_light_count; i++) {
        Vec3f transformed_light;
        vec3f_diff(transformed_light, qsl_point_light_pool[i].position ,position);

        f32 dist = vec3_mag(transformed_light);

        if (dist < smallest_dist) {
            smallest_dist = dist;
            qsl_second_nearest_pl = nearest_pl;
            nearest_pl = &qsl_point_light_pool[i];
        }
    }

    return nearest_pl;
}

point_light * qsl_pl_nearest_exclude(Vec3f position, point_light * exclude_light) {
    f32 smallest_dist = 99999.0f;
    point_light * nearest_pl = NULL;

    for(int i=0; i<qsl_point_light_count; i++) {
        if (exclude_light == &qsl_point_light_pool[i]) {continue;}
        Vec3f transformed_light;
        vec3f_diff(transformed_light, qsl_point_light_pool[i].position ,position);

        f32 dist = vec3_mag(transformed_light);

        if (dist < smallest_dist) {
            smallest_dist = dist;
            qsl_second_nearest_pl = nearest_pl;
            nearest_pl = &qsl_point_light_pool[i];
        }
    }

    return nearest_pl;
}

vector_s8 qsl_pl_direction(Vec3f position, point_light * pl) {
    Vec3f light_direction;
    vec3f_diff(light_direction,pl->position,position);
    vec3f_normalize(light_direction);

    return vec3f_to_vector_s8(light_direction);
}

color_u8 qsl_pl_color(Vec3f position, point_light * pl) {
    Vec3f transformed_light;
    vec3f_diff(transformed_light,pl->position,position);

    f32 dist = vec3_mag(transformed_light);
    f32 brightness = pl->brightness;
    color_u8 light_here;
    light_here.r = CLAMP_0(255 - (dist/brightness))*(pl->color.r/255.0f);
    light_here.g = CLAMP_0(255 - (dist/brightness))*(pl->color.g/255.0f);
    light_here.b = CLAMP_0(255 - (dist/brightness))*(pl->color.b/255.0f);
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

color_u8 qsl_plane_color(Vec3f position) {
    color_u8 plane_color;

    for (int i = 0; i < qsl_plane_light_count; i++) {
        if ((position[0] > qsl_plane_light_pool[i].x1) && (position[0] < qsl_plane_light_pool[i].x2)) {
            if ((position[2] > qsl_plane_light_pool[i].z1) && (position[2] < qsl_plane_light_pool[i].z2)) {
                f32 base_y = qsl_plane_light_pool[i].y;
                f32 light_level = CLAMP_255(CLAMP_0(255.0f-(position[1]-base_y)/qsl_plane_light_pool[i].brightness));

                plane_color.r = CLAMP_255(CLAMP_0((qsl_plane_light_pool[i].color.r/127.0f) * light_level));
                plane_color.g = CLAMP_255(CLAMP_0((qsl_plane_light_pool[i].color.g/127.0f) * light_level));
                plane_color.b = CLAMP_255(CLAMP_0((qsl_plane_light_pool[i].color.b/127.0f) * light_level));
                return plane_color;
            }
        }
    }

    // not above any light plane
    plane_color.r = 0;
    plane_color.g = 0;
    plane_color.b = 0;
    return plane_color;
}

/*
color_u8 qsl_color_env(Vec3f position, Vec3f point_normal, point_light * pl) {
    f32 sun_coverage = CLAMP_0(vec3f_dot(point_normal,qsl_global_sun_direction));
    color_u8 sun_color = qsl_sun_color(position);

    color_u8 ambient = qsl_ambient_color(position);

    f32 brightness = pl->brightness;
    color_u8 point_col = pl->color;
    Vec3f transformed_light;
    vec3f_diff(transformed_light, pl->position, position);
    f32 dist = vec3_mag(transformed_light);
    vec3f_normalize(transformed_light);
    f32 surf_coverage = CLAMP_0(vec3f_dot(point_normal,transformed_light));

    color_u8 light_here;
    light_here.r = CLAMP_255(ambient.r + (sun_coverage*sun_color.r) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.r/127.0f) );
    light_here.g = CLAMP_255(ambient.g + (sun_coverage*sun_color.g) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.g/127.0f) );
    light_here.b = CLAMP_255(ambient.b + (sun_coverage*sun_color.b) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.b/127.0f) );
    return light_here;
}
*/

color_u8 qsl_color_env(Vec3f position, Vec3f point_normal, point_light * pl) {
    f32 sun_coverage = CLAMP_0(vec3f_dot(point_normal,qsl_global_sun_direction));
    color_u8 sun_color = qsl_sun_color(position);

    color_u8 ambient = qsl_ambient_color(position);

    f32 brightness = pl->brightness;
    color_u8 point_col = pl->color;
    Vec3f transformed_light;
    vec3f_diff(transformed_light, pl->position, position);
    f32 dist = vec3_mag(transformed_light);
    vec3f_normalize(transformed_light);
    f32 surf_coverage = (1.0f + vec3f_dot(point_normal,transformed_light))/2.0f;

    color_u8 light_here;
    light_here.r = CLAMP_255(ambient.r + (sun_coverage*sun_color.r) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.r/127.0f) );
    light_here.g = CLAMP_255(ambient.g + (sun_coverage*sun_color.g) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.g/127.0f) );
    light_here.b = CLAMP_255(ambient.b + (sun_coverage*sun_color.b) + CLAMP_0(255 - (dist/brightness))*surf_coverage*(point_col.b/127.0f) );
    return light_here;
}

u8 penis_debug = 0;

void qsl_init_vtx_list(Vtx * terrain, int size) {
    for (int i = 0; i < size; i++) {
        curr_qsl_dl->addr[qsl_vertex_index].normal[0] = terrain[i].n.n[0];
        curr_qsl_dl->addr[qsl_vertex_index].normal[1] = terrain[i].n.n[1];
        curr_qsl_dl->addr[qsl_vertex_index].normal[2] = terrain[i].n.n[2];
        vec3f_normalize(curr_qsl_dl->addr[qsl_vertex_index].normal);

        curr_qsl_dl->addr[qsl_vertex_index].position[0] = terrain[i].v.ob[0];
        curr_qsl_dl->addr[qsl_vertex_index].position[1] = terrain[i].v.ob[1];
        curr_qsl_dl->addr[qsl_vertex_index].position[2] = terrain[i].v.ob[2];

        curr_qsl_dl->addr[qsl_vertex_index].pointer = NULL;
        qsl_vertex_index ++;
    }
};

void qsl_update_vtx_list_light(Vtx * terrain, int size) {
    if (qsl_point_light_count==0) {return;}

    for (int i = 0; i < size; i++) {
        point_light * pl = qsl_pl_nearest(curr_qsl_dl->addr[qsl_vertex_index].position);
        color_u8 color = qsl_color_env(curr_qsl_dl->addr[qsl_vertex_index].position, curr_qsl_dl->addr[qsl_vertex_index].normal, pl);
        terrain[i].v.cn[0] = color.r;
        terrain[i].v.cn[1] = color.g;
        terrain[i].v.cn[2] = color.b;
        qsl_vertex_index++;
    }
}

void qsl_update_vtx_list_wiggle(Vtx * terrain, int size) {
    for (int i = 0; i < size; i++) {
        s16 x = terrain[i].v.ob[0];
        s16 z = terrain[i].v.ob[2];
        terrain[i].v.ob[1] = curr_qsl_dl->addr[qsl_vertex_index].position[1] + sins((x + z + gGlobalTimer)*0x400) * 30.0f;
        qsl_vertex_index++;
    }
}

void qsl_update_vtx_list_rope(Vtx * terrain, int size) {
    for (int i = 0; i < size; i++) {
        rope_joint * my_joint;
        if (curr_qsl_dl->addr[qsl_vertex_index].pointer == NULL) {
            // If this vertex is not associated with any joint, find the closest one
            my_joint = rope_nearest_joint(curr_qsl_dl->addr[qsl_vertex_index].position);
            curr_qsl_dl->addr[qsl_vertex_index].pointer = my_joint;
        } else {
            my_joint = curr_qsl_dl->addr[qsl_vertex_index].pointer;
        }

        if (gMoveSpeed==1) {
            terrain[i].v.ob[1] = curr_qsl_dl->addr[qsl_vertex_index].position[1] + my_joint->y;
        } else {
            terrain[i].v.ob[1] = curr_qsl_dl->addr[qsl_vertex_index].position[1] + my_joint->y;
        }

        //Vec3f pos = {curr_qsl_dl->addr[qsl_vertex_index].position[0], terrain[i].v.ob[1], curr_qsl_dl->addr[qsl_vertex_index].position[2]};
        //point_light * pl = qsl_pl_nearest(pos);
        //color_u8 color = qsl_color_env(pos, curr_qsl_dl->addr[qsl_vertex_index].normal, pl);
        //terrain[i].v.cn[0] = color.r;
        //terrain[i].v.cn[1] = color.g;
        //terrain[i].v.cn[2] = color.b;

        qsl_vertex_index++;
    }
}

void qsl_update_vtx_list_camera_alpha(Vtx * terrain, int size) {
    for (int i = 0; i < size; i++) {
        Vec3f vert_to_mario;
        vec3f_diff(vert_to_mario,gMarioState->pos,curr_qsl_dl->addr[qsl_vertex_index].position);
        vec3f_normalize(vert_to_mario);
        f32 dot1 = vec3f_dot(curr_qsl_dl->addr[qsl_vertex_index].normal, vert_to_mario);

        if (dot1 > 0) {
            qsl_dl_flagged = TRUE;
        }
        qsl_vertex_index++;
    }
}

void qsl_update_vertex_iterator_thread10(void) {
    u32 * command_read;
    int qsl_gfx_stack_level = 0;
    u8 end_of_list = FALSE;
    qsl_vertex_index = 0;
    u32 * qsl_gfx_stack[40];

    set_vblank_handler(4, &gQuasilightVblankHandler, &gQuasilightVblankQueue, (OSMesg) 1);

    //Vec3f zeroo = {0,0,0};
    //color_u8 col = {255,255,255};
    //qsl_create_pl(zeroo,col,4.0f,NULL);
//
    //Vec3f plac = {700,0,0};
    //color_u8 col2 = {255,0,0};
    //qsl_create_pl(plac,col2,4.0f,NULL);

    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);

    while (TRUE) {

        for (int i = 0; i<qsl_dl_count; i++) {
            qsl_dl_flagged = FALSE;
            curr_qsl_dl = &qsl_dl_pool[i];
            qsl_vertex_index = 0;

            command_read = segmented_to_virtual(qsl_dl_pool[i].dl);
            end_of_list = FALSE;
            qsl_gfx_stack_level = 0;

            while(!end_of_list) {
                if  ( ((*command_read)>>24) == G_DL)  {
                    qsl_gfx_stack[qsl_gfx_stack_level] = command_read+2;
                    qsl_gfx_stack_level++;

                    command_read = segmented_to_virtual(*(command_read+1));
                }
                if ( ((*command_read)>>24) == G_VTX) {
                    int size = (((*command_read)>>12) & 0xFF);
                    qsl_dl_pool[i].func( segmented_to_virtual(*(command_read+1)), size);

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

            struct GraphNodeGenerated * geo_asm = curr_qsl_dl->node;
            geo_asm->qflags = qsl_dl_flagged;
        }

        osRecvMesg(&gQuasilightVblankQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
    }
}

void qsl_add_dl_to_iterator(Gfx * dl_to_add, void (*func_to_add)(Vtx * terrain, int size), struct GraphNode *node) {
    u32 * command_read = segmented_to_virtual(dl_to_add);
    int qsl_gfx_stack_level = 0;
    u8 end_of_list = FALSE;
    u32 * qsl_gfx_stack[40];
    u32 total_size = 0;

    // Calculate size and allocate memory
    while(!end_of_list) {
        if  ( ((*command_read)>>24) == G_DL)  {
            qsl_gfx_stack[qsl_gfx_stack_level] = command_read+2;
            qsl_gfx_stack_level++;
            command_read = segmented_to_virtual(*(command_read+1));
        }
        if ( ((*command_read)>>24) == G_VTX) {
            int size = (((*command_read)>>12) & 0xFF);
            total_size += (size * sizeof(float_vertex));
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

    curr_qsl_dl = &qsl_dl_pool[qsl_dl_count];
    qsl_dl_pool[qsl_dl_count].dl = dl_to_add;
    qsl_dl_pool[qsl_dl_count].func = func_to_add;
    qsl_dl_pool[qsl_dl_count].node = node;
    qsl_dl_pool[qsl_dl_count].addr = allocate(total_size);

    qsl_gfx_stack_level = 0;
    end_of_list = FALSE;
    command_read = segmented_to_virtual(dl_to_add);
    qsl_vertex_index = 0;

    // Populate allocated memory with vertices
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

    if (qsl_dl_count == 0) {
        osStartThread(&gQuasilightThread);
    }
    qsl_dl_count++;
}

void qsl_reset(void) {
    osStopThread(&gQuasilightThread);
    osDestroyThread(&gQuasilightThread);
    create_thread(&gQuasilightThread, THREAD_10_QUASILIGHT, qsl_update_vertex_iterator_thread10, NULL, gThread10Stack + THREAD10_STACK, 1);
    qsl_point_light_count = 0;
    qsl_plane_light_count = 0;
    for (int i = 0; i < qsl_dl_count; i++) {
        deallocate(qsl_dl_pool[i].addr);
    }
    qsl_dl_count = 0;
}

void qsl_process_object_light(Vec3f pos, struct Object * obj) {
    if (qsl_point_light_count==0) {return NULL;}

    if ((obj->pl)||(obj->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD)) {
        //This object IS a light source or is billboarded!
        return NULL;
    }

    Gfx *gfxlist = alloc_display_list(sizeof(*gfxlist)*8);
    Lights4 *dir_light = alloc_display_list(sizeof(*dir_light));

    point_light * pl = qsl_pl_nearest(pos);
    if (pl==NULL) {return NULL;}
    point_light * pl2 = qsl_pl_nearest_exclude(pos,pl);

    vector_s8 point_dir = qsl_pl_direction(pos, pl);
    color_u8 point_col = qsl_pl_color(pos, pl);

    vector_s8 point_dir_2 = qsl_pl_direction(pos, pl2);
    color_u8 point_col_2 = qsl_pl_color(pos, pl2);

    color_u8 amb = qsl_ambient_color(pos);
    vector_s8 sun_dir = qsl_sun_direction(pos);
    color_u8 sun_col = qsl_sun_color(pos);

    color_u8 plane_col = qsl_plane_color(pos);

    dir_light->a.l.col[0] =  amb.r;
    dir_light->a.l.col[1] =  amb.g;
    dir_light->a.l.col[2] =  amb.b;
    dir_light->a.l.colc[0] = amb.r;
    dir_light->a.l.colc[1] = amb.g;
    dir_light->a.l.colc[2] = amb.b;

    dir_light->l[0].l.dir[0]  = point_dir.x;
    dir_light->l[0].l.dir[1]  = point_dir.y;
    dir_light->l[0].l.dir[2]  = point_dir.z;
    dir_light->l[0].l.col[0]  = point_col.r;
    dir_light->l[0].l.col[1]  = point_col.g;
    dir_light->l[0].l.col[2]  = point_col.b;
    dir_light->l[0].l.colc[0] = point_col.r;
    dir_light->l[0].l.colc[1] = point_col.g;
    dir_light->l[0].l.colc[2] = point_col.b;

    dir_light->l[1].l.dir[0]  = sun_dir.x;
    dir_light->l[1].l.dir[1]  = sun_dir.y;
    dir_light->l[1].l.dir[2]  = sun_dir.z;
    dir_light->l[1].l.col[0]  = sun_col.r;
    dir_light->l[1].l.col[1]  = sun_col.g;
    dir_light->l[1].l.col[2]  = sun_col.b;
    dir_light->l[1].l.colc[0] = sun_col.r;
    dir_light->l[1].l.colc[1] = sun_col.g;
    dir_light->l[1].l.colc[2] = sun_col.b;

    dir_light->l[2].l.dir[0]  = 0;
    dir_light->l[2].l.dir[1]  = -127;
    dir_light->l[2].l.dir[2]  = 0;
    dir_light->l[2].l.col[0]  = plane_col.r;
    dir_light->l[2].l.col[1]  = plane_col.g;
    dir_light->l[2].l.col[2]  = plane_col.b;
    dir_light->l[2].l.colc[0] = plane_col.r;
    dir_light->l[2].l.colc[1] = plane_col.g;
    dir_light->l[2].l.colc[2] = plane_col.b;

    dir_light->l[3].l.dir[0]  = point_dir_2.x;
    dir_light->l[3].l.dir[1]  = point_dir_2.y;
    dir_light->l[3].l.dir[2]  = point_dir_2.z;
    dir_light->l[3].l.col[0]  = point_col_2.r;
    dir_light->l[3].l.col[1]  = point_col_2.g;
    dir_light->l[3].l.col[2]  = point_col_2.b;
    dir_light->l[3].l.colc[0] = point_col_2.r;
    dir_light->l[3].l.colc[1] = point_col_2.g;
    dir_light->l[3].l.colc[2] = point_col_2.b;


    gSPSetGeometryMode(&gfxlist[0], G_LIGHTING);
    gSPNumLights(&gfxlist[1],NUMLIGHTS_4);
    gSPLight(&gfxlist[2],&(*dir_light).l[0],1);	
    gSPLight(&gfxlist[3],&(*dir_light).l[1],2);
    gSPLight(&gfxlist[4],&(*dir_light).l[2],3);
    gSPLight(&gfxlist[5],&(*dir_light).l[3],4);
    gSPLight(&gfxlist[6],&(*dir_light).a,5);
    gSPEndDisplayList(&gfxlist[7]);

    geo_append_display_list(gfxlist, LAYER_OPAQUE);
    geo_append_display_list(gfxlist, LAYER_ALPHA);
    geo_append_display_list(gfxlist, LAYER_TRANSPARENT);
}

/* GEO ASMs
All these functions apply to whatever model / object you put them on.
*/
Gfx *geo_terrain_use_global_light(s32 callContext, struct GraphNode *node, Mat4 *mtx) {
    //return NULL;
    if (callContext == GEO_CONTEXT_RENDER) {
        Vec3f object_pos = {0.0f,0.0,0.0f};

        Gfx *gfxlist = alloc_display_list(sizeof(*gfxlist)*6);
        Lights1 *dir_light = alloc_display_list(sizeof(*dir_light));

        color_u8 amb = qsl_ambient_color(object_pos);
        vector_s8 sun_dir = qsl_sun_direction(object_pos);
        color_u8 sun_col = qsl_sun_color(object_pos);

        dir_light->a.l.col[0] =  amb.r;
        dir_light->a.l.col[1] =  amb.g;
        dir_light->a.l.col[2] =  amb.b;
        dir_light->a.l.colc[0] = amb.r;
        dir_light->a.l.colc[1] = amb.g;
        dir_light->a.l.colc[2] = amb.b;

        dir_light->l[1].l.dir[0]  = sun_dir.x;
        dir_light->l[1].l.dir[1]  = sun_dir.y;
        dir_light->l[1].l.dir[2]  = sun_dir.z;
        dir_light->l[1].l.col[0]  = sun_col.r;
        dir_light->l[1].l.col[1]  = sun_col.g;
        dir_light->l[1].l.col[2]  = sun_col.b;
        dir_light->l[1].l.colc[0] = sun_col.r;
        dir_light->l[1].l.colc[1] = sun_col.g;
        dir_light->l[1].l.colc[2] = sun_col.b;


        gSPClearGeometryMode(&gfxlist[0], G_LIGHTING);
        gSPNumLights(&gfxlist[1],NUMLIGHTS_1);
        gSPLight(&gfxlist[2],&(*dir_light).l[0],1);	
        gSPLight(&gfxlist[3],&(*dir_light).a,2);
        gSPEndDisplayList(&gfxlist[4]);

        geo_append_display_list(gfxlist, LAYER_OPAQUE);
        geo_append_display_list(gfxlist, LAYER_ALPHA);
    }

    return NULL;
}

Gfx *geo_terrain_use_point_light(s32 callContext, struct GraphNode *node, Mat4 *mtx) {

    if (callContext == GEO_CONTEXT_AREA_LOAD) {
        struct GraphNodeDisplayList * super_next = node->next;
        qsl_add_dl_to_iterator(super_next->displayList, &qsl_update_vtx_list_light, node);
    }
    if (callContext == GEO_CONTEXT_RENDER) {
        Gfx *gfxlist = alloc_display_list(sizeof(*gfxlist)*6);
        gSPClearGeometryMode(&gfxlist[0], G_LIGHTING);
        gSPEndDisplayList(&gfxlist[1]);

        geo_append_display_list(gfxlist, LAYER_OPAQUE);
        geo_append_display_list(gfxlist, LAYER_ALPHA);
    }

    return NULL;
}

Gfx *geo_terrain_wiggle(s32 callContext, struct GraphNode *node, Mat4 *mtx) {

    if (callContext == GEO_CONTEXT_AREA_LOAD) {
        struct GraphNodeDisplayList * super_next = node->next;
        qsl_add_dl_to_iterator(super_next->displayList, &qsl_update_vtx_list_wiggle, node);
    }
    if (callContext == GEO_CONTEXT_RENDER) {

    }

    return NULL;
}


Gfx *geo_terrain_rope(s32 callContext, struct GraphNode *node, Mat4 *mtx) {

    if (callContext == GEO_CONTEXT_AREA_LOAD) {
        struct GraphNodeDisplayList * super_next = node->next;
        qsl_add_dl_to_iterator(super_next->displayList, &qsl_update_vtx_list_rope, node);
    }
    if (callContext == GEO_CONTEXT_RENDER) {
        Gfx *gfxlist = alloc_display_list(sizeof(*gfxlist)*6);
        gSPClearGeometryMode(&gfxlist[0], G_LIGHTING);
        gSPEndDisplayList(&gfxlist[1]);

        geo_append_display_list(gfxlist, LAYER_OPAQUE);
        geo_append_display_list(gfxlist, LAYER_ALPHA);
    }

    return NULL;
}

Gfx *geo_terrain_camera_alpha(s32 callContext, struct GraphNode *node, Mat4 *mtx) {
    struct GraphNodeDisplayList * super_next = node->next;

    if (callContext == GEO_CONTEXT_AREA_LOAD) {
        qsl_add_dl_to_iterator(super_next->displayList, &qsl_update_vtx_list_camera_alpha, node);
    }
    if (callContext == GEO_CONTEXT_RENDER) {
        struct GraphNodeGenerated * geo_asm = node;

        if (geo_asm->qflags == 1) {
            if (geo_asm->parameter > 0) {
                geo_asm->parameter-=2;
            }
        } else {
            if (geo_asm->parameter < 200) {
                geo_asm->parameter+=2;
            }
        }

        if (geo_asm->parameter == 0) {
            SET_GRAPH_NODE_LAYER(super_next->node.flags, LAYER_OPAQUE);
        } else {
            SET_GRAPH_NODE_LAYER(super_next->node.flags, LAYER_TRANSPARENT);

            Gfx *gfxlist = alloc_display_list(sizeof(*gfxlist)*6);
            gDPSetEnvColor(&gfxlist[0], 255, 255, 255, 255-geo_asm->parameter);
            gSPEndDisplayList(&gfxlist[1]);
            geo_append_display_list(gfxlist, LAYER_TRANSPARENT);
        }
    }

    return NULL;
}