#ifndef LERP_H
#define LERP_H

extern s32 approach_angle_lerp(s32 current, s32 target);
extern f32 approach_pos_lerp(f32 current, f32 target);
extern void warp_node(struct Object *node);

#endif // LERP_H
