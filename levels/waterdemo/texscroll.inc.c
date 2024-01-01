void scroll_gfx_mat_waterdemo_dl_fancy_water_layer5() {
	Gfx *mat = segmented_to_virtual(mat_waterdemo_dl_fancy_water_layer5);

	shift_s(mat, 12, PACK_TILESIZE(0, 1));

};

void scroll_waterdemo() {
	scroll_gfx_mat_waterdemo_dl_fancy_water_layer5();
};
