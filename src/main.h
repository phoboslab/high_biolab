#ifndef MAIN_H
#define MAIN_H

#include "high_impact.h"


// -----------------------------------------------------------------------------
// Button actions

typedef enum {
	A_LEFT,
	A_RIGHT,
	A_JUMP,
	A_SHOOT
} action_t;


// -----------------------------------------------------------------------------
// Global data

typedef struct {
	font_t *font;
	noise_t *noise;
	sound_t music;
	entity_ref_t player;
	
	int death_count;
	int tubes_collected;
	int tubes_total;
	float level_time;
} global_t;

extern global_t g;


// -----------------------------------------------------------------------------
// Scenes

extern scene_t scene_title;
extern scene_t scene_game;
extern scene_t scene_credits;
extern scene_t scene_stats;

void game_set_level_path(char *path);
void game_set_checkpoint(entity_ref_t checkpoint);
void game_respawn(void);

entity_t *game_spawn_particle(vec2_t pos, float vel, float vel_variance, float angle, float angle_variance, anim_def_t *sheet);

#endif