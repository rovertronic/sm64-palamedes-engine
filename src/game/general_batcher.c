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
#include "behavior_data.h"
#include "ingame_menu.h"
#include "general_batcher.h"

void general_batcher_batch_pos(Gfx * material, Gfx * shape, batchable * start) {
    batchable * cur = start;
    gSPDisplayList(gDisplayListHead++, material);
    while (cur != NULL) {
        Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));
        Vec3f * pos = cur->pos;
        guTranslate(matrix, pos[0][0],pos[0][1],pos[0][2]);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        gSPDisplayList(gDisplayListHead++, shape);
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
        cur = cur->next;
    }
}

void general_batcher_batch_mtx(Gfx * material, Gfx * shape, batchable * start) {
    batchable * cur = start;
    gSPDisplayList(gDisplayListHead++, material);
    while (cur != NULL) {
        Mat4 * floatMatrix = cur->transform;
        Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));
        mtxf_to_mtx(matrix,floatMatrix);
        gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
        gSPDisplayList(gDisplayListHead++, shape);
        gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
        cur = cur->next;
    }
}