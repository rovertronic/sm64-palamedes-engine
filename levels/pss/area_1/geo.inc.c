#include "src/game/envfx_snow.h"

const GeoLayout pss_area_1_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ASM(0, geo_terrain_chowder),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, pss_dl_aretroslide_mesh_layer_1),
		GEO_ASM(0, geo_terrain_camera_alpha),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, pss_dl_baretroslide_001_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, 2922, 6511, -6057, pss_dl_latorn_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, 2922, 6511, -5205, pss_dl_latorn_001_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -3603, 5015, -6057, pss_dl_latorn_002_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -5700, 4800, -5421, pss_dl_latorn_003_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -6624, 4431, -3106, pss_dl_latorn_004_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -6625, 4022, -2, pss_dl_latorn_005_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -5063, 3592, 2359, pss_dl_latorn_006_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -5931, -3851, 895, pss_dl_latorn_007_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -5931, -3984, 2301, pss_dl_latorn_008_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -5931, -4117, 3701, pss_dl_latorn_009_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -6784, -4117, 3701, pss_dl_latorn_010_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -6784, -3984, 2301, pss_dl_latorn_011_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, -6784, -3851, 895, pss_dl_latorn_012_mesh_layer_1),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_ALPHA, 378, -1549, -5014, pss_dl_pope_mesh_layer_4),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_ALPHA, -2530, -3020, -4947, pss_dl_pope_001_mesh_layer_4),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_ALPHA, -4777, -3251, -2953, pss_dl_pope_002_mesh_layer_4),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, 744, 5912, -5747, pss_dl_zdark_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, 325, 5701, -5703, pss_dl_zdark_001_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, 157, 5738, -5868, pss_dl_zdark_002_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, 51, 5679, -5261, pss_dl_zdark_003_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -657, 5603, -5674, pss_dl_zdark_004_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -1002, 5393, -5932, pss_dl_zdark_005_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -1084, 5335, -5111, pss_dl_zdark_006_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -1345, 5236, -5504, pss_dl_zdark_007_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -1823, 5233, -5873, pss_dl_zdark_008_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -6149, 4045, -1238, pss_dl_zdark_009_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -6464, 3999, -1250, pss_dl_zdark_010_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -5820, 4061, -1435, pss_dl_zdark_011_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -5802, 4070, -1772, pss_dl_zdark_012_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -5923, 3727, -254, pss_dl_zdark_013_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -6366, -3543, -1315, pss_dl_zdark_014_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -6412, -3710, -801, pss_dl_zdark_015_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -6435, -3484, -631, pss_dl_zdark_016_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -6036, -3619, -605, pss_dl_zdark_017_mesh_layer_5),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_TRANSPARENT, -6285, -3595, -834, pss_dl_zdark_018_mesh_layer_5),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout pss_area_1[] = {
	GEO_NODE_SCREEN_AREA(10, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2),
	GEO_OPEN_NODE(),
		GEO_ZBUFFER(0),
		GEO_OPEN_NODE(),
			GEO_NODE_ORTHO(100.0000),
			GEO_OPEN_NODE(),
				GEO_BACKGROUND_COLOR(0x0001),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_ZBUFFER(1),
		GEO_OPEN_NODE(),
			GEO_CAMERA_FRUSTUM_WITH_FUNC(45.0000, 100, 30000, geo_camera_fov),
			GEO_OPEN_NODE(),
				GEO_CAMERA(CAMERA_MODE_8_DIRECTIONS, 0, 0, 0, 0, -100, 0, geo_camera_main),
				GEO_OPEN_NODE(),
					GEO_BRANCH(1, pss_area_1_geo),
					GEO_RENDER_OBJ(),
					GEO_ASM(ENVFX_MODE_NONE, geo_envfx_main),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
