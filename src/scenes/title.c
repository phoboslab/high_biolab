#include "../main.h"

static image_t *img_biolab;
static image_t *img_disaster;
static image_t *img_player;
static sound_source_t *sound_intro;

static bool intro_sound_played = false;

static void init(void) {
	img_biolab = image("assets/title-biolab.qoi");
	img_disaster = image("assets/title-disaster.qoi");
	img_player = image("assets/title-player.qoi");
	sound_intro = sound_source("assets/sounds/intro.qoa");
	intro_sound_played = false;
}

static void update(void) {
	scene_base_update();
	
	if (engine.time > 0.5 && !intro_sound_played) {
		sound_play(sound_intro);
		intro_sound_played = true;
	}

	if (input_pressed(A_SHOOT) || input_pressed(A_JUMP)) {
		sound_set_time(g.music, 0);
		sound_set_volume(g.music, 1.0);
		sound_unpause(g.music);

		game_set_level_path("assets/levels/biolab-1.json");
		engine_set_scene(&scene_game);
	}
}

static void draw(void) {
	scene_base_draw();

	float d = engine.time - 1;
	image_draw(img_biolab,   vec2(scale(clamp(d*d*-d, 0, 1), 1.0, 0,-160,  44), 26));
	image_draw(img_disaster, vec2(scale(clamp(d*d*-d, 0, 1), 1.0, 0, 300,  44), 70));
	image_draw(img_player,   vec2(scale(clamp(d*d*-d, 0, 1), 0.5, 0, 240, 166), 56));
	
	if (d > 2 || (int)(d * 2) % 2 == 0) {
		font_draw(g.font, vec2(120, 140), "Press X or C to Play", FONT_ALIGN_CENTER);
	}
}

scene_t scene_title = {
	.init = init,
	.update = update,
	.draw = draw,
};
