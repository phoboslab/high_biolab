#include "../main.h"

EDITOR_SIZE(40, 32);
EDITOR_RESIZE(false);
EDITOR_COLOR(81, 132, 188);

static anim_def_t *anim_idle;
static anim_def_t *anim_shards;
static sound_source_t *sound_impact;
static sound_source_t *sound_shatter;

static void load(void) {
	image_t *sheet = image("assets/sprites/glass-dome.qoi");

	anim_idle   = anim_def(sheet, vec2i(40, 32), 1.0, {0});

	image_t *sheet_shards = image("assets/sprites/glass-shards.qoi");
	anim_shards = anim_def(sheet_shards, vec2i(4, 4), 5.0, {0,1,2,3});

	sound_impact = sound_source("assets/sounds/glass-impact.qoa");
	sound_shatter = sound_source("assets/sounds/glass-shatter.qoa");
}

static void init(entity_t *self) {
	self->anim = anim(anim_idle);
	self->size = vec2(40, 32);
	self->health = 80;

	self->group = ENTITY_GROUP_BREAKABLE;
	self->check_against = ENTITY_GROUP_NONE;
	self->physics = ENTITY_PHYSICS_FIXED;
}

static void damage(entity_t *self, entity_t *other, float damage) {
	sound_play(sound_impact);
	
	for (int i = 0; i < 3; i++) {
		game_spawn_particle(other->pos, 120, 30, 0, M_PI, anim_shards);
	}
	entity_base_damage(self, other, damage);
}

static void kill(entity_t *self) {
	sound_play(sound_shatter);
	for (int i = 0; i < 100; i++) {
		vec2_t spawn_pos = vec2(
			rand_float(self->pos.x, self->pos.x + self->size.x),
			rand_float(self->pos.y, self->pos.y + self->size.y)
		);
		game_spawn_particle(spawn_pos, 120, 30, 0, M_PI, anim_shards);
	}
}

entity_vtab_t entity_vtab_glass_dome = {
	.load = load,
	.init = init,
	.damage = damage,
	.kill = kill,
};
