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

static Gfx dl_edging_shadow_start[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_ZB_XLU_DECAL, G_RM_NOOP2),
    gsSPClearGeometryMode(G_LIGHTING),
    gsSPSetGeometryMode(G_ZBUFFER),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPEndDisplayList(),
};

static Gfx dl_edging_shadow_end[] = {
    gsDPPipeSync(),
    gsDPSetRenderMode(G_RM_OPA_SURF, G_RM_OPA_SURF2),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPClearGeometryMode(G_ZBUFFER),
    gsSPTexture(0, 0, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetEnvColor(0xFF, 0xFF, 0xFF, 0xFF),
    gsSPEndDisplayList(),
};

extern Gfx dl_super_shadow[];
s32 edging_shadow_vtx_count;

#define VERTCOUNT 12

void visual_surface_display_size(Vtx *verts, s32 size) {
    s32 vts = size;
    s32 vtl = 0;
    s32 count = VERTCOUNT;
    s32 ntx = 0;

    while (vts > 0) {
        if (count == VERTCOUNT) {
            ntx = MIN(VERTCOUNT, vts);
            gSPVertex(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(verts + (size - vts)), ntx, 0);
            count = 0;
            vtl   = VERTCOUNT;
        }

        if (vtl >= 6) {
            gSP2Triangles(gDisplayListHead++, (count + 0),
                                              (count + 1),
                                              (count + 2), 0x0,
                                              (count + 3),
                                              (count + 4),
                                              (count + 5), 0x0);
            vts   -= 6;
            vtl   -= 6;
            count += 6;
        } else if (vtl >= 3) {
            gSP1Triangle(gDisplayListHead++, (count + 0),
                                             (count + 1),
                                             (count + 2), 0x0);
            vts   -= 3;
            vtl   -= 3;
            count += 3;
        }
    }
}

void generate_edging_shadow_vtx(s32 x, s32 y, s32 z, Vtx *verts) {
    struct SurfaceNode *node;
    struct Surface *surf;

    if (is_outside_level_bounds(x, z)) return;

    s32 cellX = GET_CELL_COORD(x);
    s32 cellZ = GET_CELL_COORD(z);

    for (u8 i = 0; i < 4; i++) {
        switch (i) {
            case 0: node = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WALLS ].next; break;
            case 1: node =  gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_WALLS ].next; break;
            case 2: node = gDynamicSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS].next; break;
            case 3: node =  gStaticSurfacePartition[cellZ][cellX][SPATIAL_PARTITION_FLOORS].next; break;
        }

        while (node != NULL) {
            surf = node->surface;
            node = node->next;

            s32 y1 = surf->vertex1[1];
            s32 y2 = surf->vertex2[1];
            s32 y3 = surf->vertex3[1];

            switch (i) {
                case 0:
                case 1:
                    // Wall cutoff
                    if (y1 > y) {
                        y1 = y;
                    }
                    if (y2 > y) {
                        y2 = y;
                    }
                    if (y3 > y) {
                        y3 = y;
                    }
                    break;
                case 2:
                case 3:
                    // Floor cutoff
                    if (y < surf->lowerY) {
                        continue;
                    }

                    // Skip triangle if there's going to be UV overflow
                    s32 test;
                    test = (surf->vertex1[0]-x)*7+512;
                    if ((test > 32767) || (test < -32767)) {
                        continue;
                    }
                    test = (surf->vertex2[0]-x)*7+512;
                    if ((test > 32767) || (test < -32767)) {
                        continue;
                    }
                    test = (surf->vertex3[0]-x)*7+512;
                    if ((test > 32767) || (test < -32767)) {
                        continue;
                    }
                    test = (surf->vertex1[2]-z)*7+512;
                    if ((test > 32767) || (test < -32767)) {
                        continue;
                    }
                    test = (surf->vertex2[2]-z)*7+512;
                    if ((test > 32767) || (test < -32767)) {
                        continue;
                    }
                    test = (surf->vertex3[2]-z)*7+512;
                    if ((test > 32767) || (test < -32767)) {
                        continue;
                    }
                    break;
            }
                                                                                                            // UV PROJECTION
            make_vertex(verts, (edging_shadow_vtx_count + 0), surf->vertex1[0], y1, surf->vertex1[2], (surf->vertex1[0]-x)*7+512, (surf->vertex1[2]-z)*7+512, 0xFF, 0xFF, 0xFF, 0xFF);
            make_vertex(verts, (edging_shadow_vtx_count + 1), surf->vertex2[0], y2, surf->vertex2[2], (surf->vertex2[0]-x)*7+512, (surf->vertex2[2]-z)*7+512, 0xFF, 0xFF, 0xFF, 0xFF);
            make_vertex(verts, (edging_shadow_vtx_count + 2), surf->vertex3[0], y3, surf->vertex3[2], (surf->vertex3[0]-x)*7+512, (surf->vertex3[2]-z)*7+512, 0xFF, 0xFF, 0xFF, 0xFF);
            edging_shadow_vtx_count += 3;
        }
    }
}

void render_edging_shadow(void) {
    if (!gSurfaceNodesAllocated
     || !gSurfacesAllocated
     || !gMarioState->marioObj) {
        return;
    }
    Vtx *verts = alloc_display_list(0x2000);

    //figure this shit out later
    //(iterate_surface_count(gMarioState->marioObj->header.gfx.posLerp[0], gMarioState->marioObj->header.gfx.posLerp[2]) * 3) * sizeof(Vtx)

    if (verts == NULL) {
        return;
    }

    edging_shadow_vtx_count = 0;

    gSPDisplayList(gDisplayListHead++, dl_edging_shadow_start);
    gSPDisplayList(gDisplayListHead++, dl_super_shadow);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 200);
    generate_edging_shadow_vtx(gMarioState->marioObj->header.gfx.posLerp[0], gMarioState->marioObj->header.gfx.posLerp[1]+50, gMarioState->marioObj->header.gfx.posLerp[2], verts);
    visual_surface_display_size(verts, edging_shadow_vtx_count);
    gSPDisplayList(gDisplayListHead++, dl_edging_shadow_end);
}