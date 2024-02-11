#include <PR/ultratypes.h>
#include "rope_constraint.h"
#include "sm64.h"
#include "game/game_init.h"
#include "game/geo_misc.h"
#include "engine/math_util.h"
#include "engine/colors.h"
#include "area.h"
#include "level_update.h"
#include "print.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "object_list_processor.h"
#include "debug_box.h"

rope_joint rope_joint_pool[30];
int rope_joint_count = 0;

rope_surface rope_surface_pool[ROPE_SURFACE_POOL_SIZE];
int rope_surface_count = 0;

#define ROPE_GRAVITY 10.0f
#define ROPE_STRENGTH 1.0f
#define ROPE_DAMPING 0.5f

rope_joint * rope_nearest_joint(Vec3f position) {
    f32 smallest_dist = 99999.0f;
    rope_joint * nearest_joint = NULL;

    for(int i=0; i<rope_joint_count; i++) {
        Vec3f transformed_joint;
        vec3f_diff(transformed_joint, rope_joint_pool[i].world_position, position);

        f32 dist = vec3_mag(transformed_joint);

        if (dist < smallest_dist) {
            smallest_dist = dist;
            nearest_joint = &rope_joint_pool[i];
        }
    }

    return nearest_joint;
}

rope_joint * init_rope_bridge(int size) {
    rope_joint * start = NULL;
    for (int i = 0; i < size; i ++) {
        rope_joint * self = &rope_joint_pool[i+rope_joint_count];
        self->prev = &rope_joint_pool[rope_joint_count+i-1];
        self->next = &rope_joint_pool[rope_joint_count+i+1];

        self->world_position[0] = 0.0f;
        self->world_position[1] = 0.0f;
        self->world_position[2] = -900.0f + (i * 200.0f);

        self->y = 0.0f;
        self->lerp_y = 0.0f;
        self->y_vel = 0.0f;
        self->pinned = FALSE;
        if (i == 0) {
            // Start of the rope
            self->prev = NULL;
            self->pinned = TRUE;
            start = self;
        }
        if (i == size-1) {
            // End of the rope
            self->next = NULL;
            self->pinned = TRUE;
        }
    }
    rope_joint_count += size;
    return start;
}

u8 did_init = FALSE;
void rope_step(void) {

    if (!did_init) {
        did_init = TRUE;
        init_rope_bridge(30);
    }

    // Update associated collision
    for (int i=0; i<rope_surface_count; i++) {
        struct Surface * surf = rope_surface_pool[i].surf;
        s16 min, max;

        f32 old_y = surf->vertex1[1];

        surf->vertex1[1] = rope_surface_pool[i].home_surf.vertex1[1] + rope_surface_pool[i].vertexs_joint[0]->y;
        surf->vertex2[1] = rope_surface_pool[i].home_surf.vertex2[1] + rope_surface_pool[i].vertexs_joint[1]->y;
        surf->vertex3[1] = rope_surface_pool[i].home_surf.vertex3[1] + rope_surface_pool[i].vertexs_joint[2]->y;
        
        Vec3f vertex1_vec3f = {rope_surface_pool[i].surf->vertex1[0],rope_surface_pool[i].surf->vertex1[1],rope_surface_pool[i].surf->vertex1[2]};

        Vec3f result;
        find_vector_perpendicular_to_plane(result, surf->vertex1, surf->vertex2, surf->vertex3);
        f32 mag = (sqr(result[0]) + sqr(result[1]) + sqr(result[2]));
        mag = 1.0f / sqrtf(mag);
        vec3_mul_val(result, mag);

        vec3f_copy((Vec3f*)&surf->normal, result);

        surf->originOffset = -vec3f_dot((Vec3f*)&surf->normal, vertex1_vec3f);

        min_max_3s(surf->vertex1[1], surf->vertex2[1], surf->vertex3[1], &min, &max);
        surf->lowerY = (min - SURFACE_VERTICAL_BUFFER);
        surf->upperY = (max + SURFACE_VERTICAL_BUFFER);

        if (gMarioState->floor == surf) {
            //Mario is standing on this tri, offset
            gMarioState->pos[1] += (surf->vertex1[1]-old_y);
            gMarioObject->oPosY += (surf->vertex1[1]-old_y);
        }
    }

    // Update rope joints
    for (int i=0; i<rope_joint_count; i++) {
        rope_joint * self = &rope_joint_pool[i];
        if (!self->pinned) {
            // Gravity
            self->y_vel -= ROPE_GRAVITY;

            f32 offset;
            // Solving joints in a random order allegedly stabilizes a physics simulation,
            // or so I've been told.
            if (random_u16()%2==0) {
                // Constraint next
                if (self->prev != NULL) {
                    offset = self->prev->y - self->y;
                    self->y_vel += offset * ROPE_STRENGTH;
                }

                // Constraint previous
                if (self->next != NULL) {
                    offset = self->next->y - self->y;
                    self->y_vel += offset * ROPE_STRENGTH;
                }
            } else {
                // Constraint previous
                if (self->next != NULL) {
                    offset = self->next->y - self->y;
                    self->y_vel += offset * ROPE_STRENGTH;
                }

                // Constraint next
                if (self->prev != NULL) {
                    offset = self->prev->y - self->y;
                    self->y_vel += offset * ROPE_STRENGTH;
                }
            }

            self->y_vel -= (self->y_vel * ROPE_DAMPING); // Damping

            f32 old_y = self->y;
            self->y += self->y_vel;
            self->lerp_y = old_y + ((self->y - old_y) * 0.5f);
        }
    }
    
}