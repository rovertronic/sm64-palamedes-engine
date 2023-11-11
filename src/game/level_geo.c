#include <ultra64.h>

#include "sm64.h"
#include "rendering_graph_node.h"
#include "mario_misc.h"
#include "skybox.h"
#include "engine/math_util.h"
#include "camera.h"
#include "envfx_snow.h"
#include "level_geo.h"
#include "quasilight.h"

/**
 * Geo function that generates a displaylist for environment effects such as
 * snow or jet stream bubbles.
 */
Gfx *geo_envfx_main(s32 callContext, struct GraphNode *node, Mat4 mtxf) {
    Vec3s marioPos;
    Vec3s camFrom;
    Vec3s camTo;
    void *particleList;
    Gfx *gfx = NULL;

    if (callContext == GEO_CONTEXT_RENDER && gCurGraphNodeCamera != NULL) {
        struct GraphNodeGenerated *execNode = (struct GraphNodeGenerated *) node;
        u32 *params = &execNode->parameter; // accessed a s32 as 2 u16s by pointing to the variable and
                                            // casting to a local struct as necessary.

        if (GET_HIGH_U16_OF_32(*params) != gAreaUpdateCounter) {
            s32 snowMode = GET_LOW_U16_OF_32(*params);

            vec3f_to_vec3s(camTo, gCurGraphNodeCamera->focus);
            vec3f_to_vec3s(camFrom, gCurGraphNodeCamera->pos);
            vec3f_to_vec3s(marioPos, gPlayerCameraState->pos);
            particleList = envfx_update_particles(snowMode, marioPos, camTo, camFrom);
            if (particleList != NULL) {
                Mtx *mtx = alloc_display_list(sizeof(*mtx));

                gfx = alloc_display_list(2 * sizeof(*gfx));
                mtxf_to_mtx(mtx, mtxf);
                gSPMatrix(&gfx[0], VIRTUAL_TO_PHYSICAL(mtx), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                gSPBranchList(&gfx[1], VIRTUAL_TO_PHYSICAL(particleList));
                SET_GRAPH_NODE_LAYER(execNode->fnNode.node.flags, LAYER_OCCLUDE_SILHOUETTE_ALPHA);
            }
            SET_HIGH_U16_OF_32(*params, gAreaUpdateCounter);
        }
    } else if (callContext == GEO_CONTEXT_AREA_INIT) {
        // Give these arguments some dummy values. Not used in ENVFX_MODE_NONE
        vec3s_copy(camTo, gVec3sZero);
        vec3s_copy(camFrom, gVec3sZero);
        vec3s_copy(marioPos, gVec3sZero);
        envfx_update_particles(ENVFX_MODE_NONE, marioPos, camTo, camFrom);
    }

    return gfx;
}

/**
 * Geo function that generates a displaylist for the skybox. Can be assigned
 * as the function of a GraphNodeBackground.
 */
Gfx *geo_skybox_main(s32 callContext, struct GraphNode *node, UNUSED Mat4 *mtx) {
    Gfx *gfx = NULL;
    struct GraphNodeBackground *backgroundNode = (struct GraphNodeBackground *) node;

    if (callContext == GEO_CONTEXT_AREA_LOAD) {
        backgroundNode->unused = 0;
#ifndef L3DEX2_ALONE
    } else if (callContext == GEO_CONTEXT_RENDER) {
        struct GraphNodeCamera *camNode = (struct GraphNodeCamera *) gCurGraphNodeRoot->views[0];
        struct GraphNodePerspective *camFrustum =
            (struct GraphNodePerspective *) camNode->fnNode.node.parent;
        gfx = create_skybox_facing_camera(0, backgroundNode->background, camFrustum->fov, gLakituState.pos, gLakituState.focus);
#endif
    }

    return gfx;
}

Lights1 defaultLight5 = gdSPDefLights1(
    0x3F, 0x3F, 0x3F, 0xFF, 0xFF, 0x00, 0x00, 0x127, 0x00
);

Gfx *geo_object_calculate_light(s32 callContext, struct GraphNode *node, Mat4 *mtx) {

    Vec3f object_pos = {(*mtx)[3][0], (*mtx)[3][1], (*mtx)[3][2]};
    //struct Object *obj = (struct Object *) node;
    //Vec3f object_pos = {obj->oPosX, obj->oPosY, obj->oPosZ};

    Gfx *gfxlist = alloc_display_list(sizeof(*gfxlist)*6);
    Lights2 *dir_light = alloc_display_list(sizeof(*dir_light));

    vector_s8 point_dir = qsl_pl_direction(object_pos);
    color_u8 point_col = qsl_pl_color(object_pos);

    color_u8 amb = qsl_ambient_color(object_pos);
    vector_s8 sun_dir = qsl_sun_direction(object_pos);
    color_u8 sun_col = qsl_sun_color(object_pos);

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


    gSPSetGeometryMode(&gfxlist[0], G_LIGHTING);
	gSPNumLights(&gfxlist[1],NUMLIGHTS_2);
	gSPLight(&gfxlist[2],&(*dir_light).l[0],1);	
	gSPLight(&gfxlist[3],&(*dir_light).l[1],2);
	gSPLight(&gfxlist[4],&(*dir_light).a,3);
    gSPEndDisplayList(&gfxlist[5]);

    geo_append_display_list(gfxlist, LAYER_OPAQUE);
    geo_append_display_list(gfxlist, LAYER_ALPHA);

    return NULL;
}