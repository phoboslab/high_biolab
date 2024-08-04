#include "../main.h"

EDITOR_SIZE(8, 5);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 155, 32);

anim_def_t *anim_idle;
anim_def_t *anim_gib;

sound_source_t *sound_explode;

static void load(void) {
	image_t *sheet = image("assets/sprites/mine.qoi");
	anim_idle = anim_def(sheet, vec2i(16, 8), 0.17, {1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,3});
	anim_gib  = anim_def(sheet, vec2i(4, 4), 1, {0});
	sound_explode = sound_source("assets/sounds/mine.qoa");
}

static void init(entity_t *self) {
	self->size = vec2(8, 5);
	self->offset = vec2(4, 3);
	self->check_against = ENTITY_GROUP_PLAYER;
	self->anim = anim(anim_idle);
	anim_goto_rand(&self->anim);
}

static void kill(entity_t *self) {
	for (int i = 0; i < 10; i++) {
		entity_t *particle = game_spawn_particle(self->pos, 120, 30, -M_PI/2, M_PI/2, anim_gib);
		if (particle) {
			particle->physics = ENTITY_PHYSICS_WORLD;
		}
	}
	sound_play(sound_explode);
}

static void touch(entity_t *self, entity_t *other) {
	entity_kill(self);
	entity_damage(other, self, 10);
}

entity_vtab_t entity_vtab_mine = {
	.init = init,
	.load = load,
	.kill = kill,
	.touch = touch,
};