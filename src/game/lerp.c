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

void warp_node(struct Object *node) {
    node->header.gfx.bothMats++;
    vec3f_copy(node->header.gfx.posLerp, node->header.gfx.pos);
    vec3s_copy(node->header.gfx.angleLerp, node->header.gfx.angle);
    vec3f_copy(node->header.gfx.scaleLerp, node->header.gfx.scale);
}

s32 absi_lerp(s32 x) {
    if (x >= 0) {
        return x;
    } else {
        return -x;
    }
}

u32 abs_angle_diff_lerp(s16 x0, s16 x1) {
    return absi_lerp((s16) (x1 - x0));
}

s32 approach_angle_lerp(s32 current, s32 target) {
    s32 diff1;
    s32 ret;
    //return target - localLerp((s16) (target - current), 0, gLerpSpeed);
    if ((diff1 = abs_angle_diff_lerp(current, target)) >= 0x2000) {
        return target;
    }
    if (gMoveSpeed == 1) {
        ret = (target + current) >> 1;
    } else {
        ret = current - (target - current) >> 1;
    }
    if ((diff1 < (absi_lerp(target - current + 0x10000))) && (diff1 < (absi_lerp(target - current - 0x10000)))) {
        return ret;
    } else {
        return ret + 0x8000;
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