#include "src/game/envfx_snow.h"

const GeoLayout test_area_1_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_ASM(0, geo_terrain_wiggle),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, test_dl_acid_mesh_layer_1),
		GEO_ASM(0, geo_terrain_use_global_light),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, test_dl_node_mesh_layer_1),
		GEO_ASM(0, geo_terrain_camera_alpha),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, test_dl_node_001_mesh_layer_5),
		GEO_ASM(0, geo_terrain_camera_alpha),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, test_dl_node_002_mesh_layer_5),
		GEO_ASM(0, geo_terrain_camera_alpha),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, test_dl_node_003_mesh_layer_5),
		GEO_ASM(0, geo_terrain_use_point_light),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, test_dl_Plane_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout test_area_1[] = {
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
					GEO_BRANCH(1, test_area_1_geo),
					GEO_RENDER_OBJ(),
					GEO_ASM(ENVFX_MODE_NONE, geo_envfx_main),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, test_dl_material_revert_render_settings),
		GEO_DISPLAY_LIST(LAYER_TRANSPARENT, test_dl_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
