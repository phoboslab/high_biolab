#include "../main.h"

static sound_t *music;
static entity_ref_t last_checkpoint = entity_ref_none();
static vec2_t initial_spawn_pos = vec2(0, 0);
static char level_path[64] = {0};
static camera_t camera;

static void init(void) {
	engine_load_level(level_path);
	engine.gravity = 240;

	for (int i = 0; i < engine.background_maps_len; i++) {
		map_t *map = engine.background_maps[i];
		map_set_anim(map, 80, 0.13, {80,81,82,83,84,85,86,87});
		map_set_anim(map, 81, 0.17, {84,83,82,81,80,87,86,85});
		map_set_anim(map, 88, 0.23, {88,89,90,91,92,93,94,95});
		map_set_anim(map, 89, 0.19, {95,94,93,92,91,90,89,88});
	}
	
	camera.offset = vec2(32, 0);
	camera.speed = 3;
	camera.min_vel = 5;

	last_checkpoint = entity_ref_none();

	entity_list_t players = entities_by_type(ENTITY_TYPE_PLAYER);
	if (players.len > 0) {
		g.player = players.entities[0];
		camera_follow(&camera, g.player, true);
		entity_t *player_ent = entity_by_ref(g.player);
		initial_spawn_pos = player_ent->pos;
	}

	g.tubes_collected = 0;
	g.death_count = 0;
	g.tubes_total = entities_by_type(ENTITY_TYPE_TEST_TUBE).len;
}

static void cleanup(void) {
	g.level_time = engine.time;
}

static void update(void) {
	scene_base_update();

	camera_update(&camera);
}

static void draw(void) {
	scene_base_draw();
	
	// Draw some debug info...
	// font_draw(g.font, vec2(2,2), 
	// 	str_format(
	// 		"total: %.2fms, update: %.2fms, draw: %.2fms\n"
	// 		"draw calls: %d, entities: %d, checks: %d", 
	// 		engine.perf.total * 1000, engine.perf.update * 1000, engine.perf.draw * 1000, 
	// 		engine.perf.draw_calls, engine.perf.entities, engine.perf.checks
	// 	),
	// 	FONT_ALIGN_LEFT
	// );
}

entity_t *game_spawn_particle(vec2_t pos, float vel, float vel_variance, float angle, float angle_variance, anim_def_t *sheet) {
	entity_t *particle = entity_spawn(ENTITY_TYPE_PARTICLE, pos);
	if (!particle) {
		return NULL;
	}
	
	particle->anim = anim(sheet);
	anim_goto_rand(&particle->anim);
	particle->anim.flip_x = rand_int(0, 1);
	particle->anim.flip_y = rand_int(0, 1);

	float a = rand_float(angle-angle_variance, angle+angle_variance);
	float v = rand_float(vel - vel_variance, vel + vel_variance);
	particle->vel = vec2_mulf(vec2_from_angle(a), v);
	return particle;
}

void game_set_checkpoint(entity_ref_t checkpoint) {
	last_checkpoint = checkpoint;
}

void game_respawn(void) {
	vec2_t pos = initial_spawn_pos;

	entity_t *respawn_pod = entity_by_ref(last_checkpoint);
	if (respawn_pod) {
		pos = vec2_add(respawn_pod->pos, vec2(11, 0));
		entity_message(respawn_pod, EM_ACTIVATE, NULL);
	}

	entity_t *player = entity_spawn(ENTITY_TYPE_PLAYER, pos);
	g.player = entity_ref(player);
	camera_follow(&camera, g.player, false);
	g.death_count++;
}

void game_set_level_path(char *path) {
	error_if(strlen(path) >= sizeof(level_path)-1, "level path too long");
	strcpy(level_path, path);
}

scene_t scene_game = {
	.init = init,
	.update = update,
	.draw = draw,
	.cleanup = cleanup
};
