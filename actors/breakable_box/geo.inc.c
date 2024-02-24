#include "src/game/envfx_snow.h"

extern Gfx breakable_box_Cube_mesh_layer_1[];

const GeoLayout breakable_box_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, breakable_box_Cube_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_END(),
};

const GeoLayout breakable_box_no_shadow_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, breakable_box_Cube_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
