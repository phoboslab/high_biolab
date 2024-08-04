#include "../main.h"

EDITOR_SIZE(8, 8);
EDITOR_RESIZE(false);
EDITOR_COLOR(81, 132, 188);

static anim_def_t *anim_idle;
static anim_def_t *anim_debris;
static sound_source_t *sound_crack;

static void load(void) {
	image_t *sheet = image("assets/sprites/crate.qoi");

	anim_idle   = anim_def(sheet, vec2i(8, 8), 1.0, {0});
	anim_debris = anim_def(sheet, vec2i(4, 4), 5.0, {2,3,6,7});

	sound_crack = sound_source("assets/sounds/crack.qoa");
}

static void init(entity_t *self) {
	self->anim = anim(anim_idle);
	self->size = vec2(8, 8);
	self->friction = vec2(4, 0);
	self->health = 5;
	self->restitution = 0.4;
	self->mass = 0.25;

	self->group = ENTITY_GROUP_BREAKABLE;
	self->check_against = ENTITY_GROUP_NONE;
	self->physics = ENTITY_PHYSICS_ACTIVE;
}

static void kill(entity_t *self) {
	sound_play(sound_crack);
	
	for (int i = 0; i < 10; i++) {
		game_spawn_particle(self->pos, 120, 30, 0, M_PI, anim_debris);
	}
}

entity_vtab_t entity_vtab_crate = {
	.load = load,
	.init = init,
	.kill = kill,
};
