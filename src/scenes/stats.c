#include "../main.h"

static void update(void) {
	scene_base_update();
	
	if (input_pressed(A_SHOOT) || input_pressed(A_JUMP)) {
		engine_set_scene(&scene_game);
	}
}

static void draw(void) {
	scene_base_draw();

	font_draw(g.font, vec2(render_size().x/2, 20), "Level Complete!", FONT_ALIGN_CENTER);

	font_draw(g.font, vec2( 98, 56), "Time:", FONT_ALIGN_RIGHT);
	font_draw(g.font, vec2(104, 56), str_format("%.2fs", g.level_time), FONT_ALIGN_LEFT);

	font_draw(g.font, vec2( 98, 68), "Tubes Collected:", FONT_ALIGN_RIGHT);
	font_draw(g.font, vec2(104, 68), str_format("%d/%d", g.tubes_collected, g.tubes_total), FONT_ALIGN_LEFT);

	font_draw(g.font, vec2( 98, 80), "Deaths:", FONT_ALIGN_RIGHT);
	font_draw(g.font, vec2(104, 80), str_format("%d", g.death_count), FONT_ALIGN_LEFT);

	font_draw(g.font, vec2(render_size().x/2, 140), "Press X or C to Proceed", FONT_ALIGN_CENTER);
}

scene_t scene_stats = {
	.update = update,
	.draw = draw,
};
