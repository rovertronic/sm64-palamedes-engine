#include "engine/math_util.h"
#include "game_init.h"
#include "quasilight.h"

/*
QUASILIGHT.C 
Quasilight is a lighting engine for Super Mario 64.
The core concept is that objects use directional lights to simulate lighting while
the environment uses vertex colors to simulate terrain lighting. Multi-threading is used
to ensure that environment lighting does not lag general gameplay.
*/

#define CLAMP_0(x) ((x < 0) ? 0 : x)

vector_s8 vec3f_to_vector_s8(Vec3f vector) {
    vector_s8 returnvec;
    returnvec.x = vector[0] * 126.0f;
    returnvec.y = vector[1] * 126.0f;
    returnvec.z = vector[2] * 126.0f;

    return returnvec;
}

vector_s8 qsl_nearest_pl_direction(Vec3f position) {
    Vec3f source = {0.0f, 0.0f, 0.0f};
    Vec3f light_direction;
    vec3f_diff(light_direction,source,position);
    vec3f_normalize(light_direction);

    return vec3f_to_vector_s8(light_direction);
}

color_s8 qsl_nearest_pl_color(Vec3f position) {
    f32 dist = vec3_mag(position);
    f32 brightness = 4.0f;
    color_s8 light_here;
    light_here.r = CLAMP_0(255 - (dist/brightness));
    light_here.g = CLAMP_0(255 - (dist/brightness));
    light_here.b = 0;
    return light_here;
}