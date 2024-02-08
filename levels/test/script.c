#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"


/* Fast64 begin persistent block [includes] */
/* Fast64 end persistent block [includes] */

#include "make_const_nonconst.h"
#include "levels/test/header.h"

/* Fast64 begin persistent block [scripts] */
/* Fast64 end persistent block [scripts] */

const LevelScript level_test_entry[] = {
	INIT_LEVEL(),
	LOAD_YAY0(0x7, _test_segment_7SegmentRomStart, _test_segment_7SegmentRomEnd), 
	LOAD_YAY0(0xa, _water_skybox_yay0SegmentRomStart, _water_skybox_yay0SegmentRomEnd), 
	LOAD_YAY0(0x08, _common0_yay0SegmentRomStart, _common0_yay0SegmentRomEnd), 
	LOAD_RAW(0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd), 
	ALLOC_LEVEL_POOL(),
	MARIO(MODEL_MARIO, 0x00000001, bhvMario), 
	JUMP_LINK(script_func_global_1), 

	/* Fast64 begin persistent block [level commands] */
	/* Fast64 end persistent block [level commands] */

	AREA(1, test_area_1),
		WARP_NODE(0x01, LEVEL_BOB, 0x01, 0x0A, WARP_NO_CHECKPOINT),
		OBJECT(MODEL_AMP, -1168, -2451, 5046, 0, 0, 0, 0x00000000, bhvCirclingAmp),
		OBJECT(MODEL_AMP, 713, -2331, 3760, 0, 0, 0, 0x00000000, bhvCirclingAmp),
		OBJECT(MODEL_AMP, 628, -2446, 2825, 0, 0, 0, 0x00000000, bhvCirclingAmp),
		OBJECT(MODEL_NONE, 5198, -3626, 1813, 0, 0, 0, 0x00000000, bhvStaticObject),
		MARIO_POS(0x01, 0, 0, -140, 0),
		OBJECT(MODEL_STAR, 3341, 2205, -6803, 0, 0, 0, 0x00000000, bhvStar),
		OBJECT(MODEL_NONE, -10685, -3348, 10, 0, 0, 0, (0x01 << 16), bhvWarp),
		TERRAIN(test_area_1_collision),
		MACRO_OBJECTS(test_area_1_macro_objs),
		STOP_MUSIC(0),
		TERRAIN_TYPE(TERRAIN_GRASS),
		/* Fast64 begin persistent block [area commands] */
		/* Fast64 end persistent block [area commands] */
	END_AREA(),

	FREE_LEVEL_POOL(),
	MARIO_POS(0x01, 0, 0, -140, 0),
	CALL(0, lvl_init_or_update),
	CALL_LOOP(1, lvl_init_or_update),
	CLEAR_LEVEL(),
	SLEEP_BEFORE_EXIT(1),
	EXIT(),
};
