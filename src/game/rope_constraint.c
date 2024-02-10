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

rope_joint rope_joint_pool[20];
int rope_joint_count = 0;

#define ROPE_GRAVITY 10.0f
#define ROPE_STRENGTH 1.0f
#define ROPE_DAMPING 0.5f

rope_joint * init_rope_bridge(int size) {
    rope_joint * start = NULL;
    for (int i = 0; i < size; i ++) {
        rope_joint * self = &rope_joint_pool[i+rope_joint_count];
        self->prev = &rope_joint_pool[rope_joint_count+i-1];
        self->next = &rope_joint_pool[rope_joint_count+i+1];
        self->y = 0.0f;
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
        init_rope_bridge(15);
    }

    for (int i=0; i<rope_joint_count; i++) {
        rope_joint * self = &rope_joint_pool[i];
        if (!self->pinned) {
            // Gravity
            self->y_vel -= ROPE_GRAVITY;

            // Constraint next
            f32 offset;
            if (self->prev != NULL) {
                offset = self->prev->y - self->y;
                self->y_vel += offset * ROPE_STRENGTH;
            }

            // Constraint previous
            if (self->next != NULL) {
                offset = self->next->y - self->y;
                self->y_vel += offset * ROPE_STRENGTH;
            }

            self->y_vel -= (self->y_vel * ROPE_DAMPING); // Damping

            self->y += self->y_vel;
        }

        print_text_fmt_int(i*16+20, 150+(self->y/10.0f), "A", 0);
    }

    if (gMarioState->controller->buttonDown & A_BUTTON) {
        rope_joint_pool[3].y_vel -= 200.0f;
    }
    if (gMarioState->controller->buttonDown & B_BUTTON) {
        rope_joint_pool[0].y -= 20.0f;
    }
}