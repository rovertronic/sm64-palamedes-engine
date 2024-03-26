#include "src/game/envfx_snow.h"

const GeoLayout retrostar_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_ALPHA, retrostar_Plane_001_mesh_layer_4),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
