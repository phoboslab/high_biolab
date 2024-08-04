#include "../main.h"

EDITOR_SIZE(8, 10);
EDITOR_RESIZE(false);
EDITOR_COLOR(81, 132, 188);

static anim_def_t *anim_idle;
static sound_source_t *sound_collect;

static void load(void) {
	image_t *sheet = image("assets/sprites/test-tube.qoi");
	anim_idle = anim_def(sheet, vec2i(8, 10), 0.1, {0,0,0,1,2,3,0,0,0,2,0,0,1,0,0});
	sound_collect = sound_source("assets/sounds/collect.qoa");
}

static void init(entity_t *self) {
	self->anim = anim(anim_idle);
	anim_goto_rand(&self->anim);
	self->size = vec2(8, 10);
	self->check_against = ENTITY_GROUP_PLAYER;
}

static void touch(entity_t *self, entity_t *other) {
	entity_kill(self);
	sound_play(sound_collect);
	g.tubes_collected++;
}

entity_vtab_t entity_vtab_test_tube = {
	.load = load,
	.init = init,
	.touch = touch,
};