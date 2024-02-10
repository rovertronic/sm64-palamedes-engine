#ifndef ROPE_CONSTRAINT_H
#define ROPE_CONSTRAINT_H

typedef struct rope_joint rope_joint;

struct rope_joint {
    rope_joint * next;
    rope_joint * prev;
    f32 y;
    f32 y_vel;
    u8 pinned;
};

extern void rope_step(void);

#endif