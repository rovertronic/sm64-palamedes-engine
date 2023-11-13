#include <ultra64.h>
#include "types.h"
#include "game_init.h"
#include "engine/math_util.h"
#include "camera.h"

#define LERP_THRESHOLD 200.0f
#define LERP_THRESHOLD_ANGLE 0x2000

static f32 localLerp(f32 current, f32 target, f32 multiplier) {
    current = current + (target - current) * multiplier;
    return current;
}

s32 abs_angle_diff_2(s16 x0, s16 x1) {
    s16 diff = x1 - x0;

    if (diff == -0x8000) {
        diff = -0x7FFF;
    }

    if (diff < 0) {
        diff = -diff;
    }

    return diff;
}

void warp_node(struct Object *node) {
    node->header.gfx.bothMats++;
    vec3f_copy(node->header.gfx.posLerp, node->header.gfx.pos);
    vec3s_copy(node->header.gfx.angleLerp, node->header.gfx.angle);
    vec3f_copy(node->header.gfx.scaleLerp, node->header.gfx.scale);
}

s32 approach_angle_lerp(s32 current, s32 target) {
    if ((abs_angle_diff_2(current, target)) >= LERP_THRESHOLD_ANGLE) {
        return target;
    }

    s32 move = localLerp((s16) (target - current), 0, gMoveSpeed);
    if (gMoveSpeed == 1) {
        return (target + move);
    } else {
        return (target - move);
    }
}

f32 approach_pos_lerp(f32 current, f32 target) {
    if (ABS(target - current) >= LERP_THRESHOLD)
        return target;
    if (gMoveSpeed == 1) {
        return current + ((target - current) * 0.5f);
    } else {
        return current - ((target - current) * 0.5f);
    }
}