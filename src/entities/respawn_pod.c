#include "../main.h"

EDITOR_SIZE(18, 16);
EDITOR_RESIZE(false);
EDITOR_COLOR(81, 132, 188);

static anim_def_t *anim_idle;
static anim_def_t *anim_activated;
static anim_def_t *anim_respawn;

static sound_source_t *sound_activated;

static void load(void) {
	image_t *sheet = image("assets/sprites/respawn-pod.qoi");

	anim_idle      = anim_def(sheet, vec2i(18, 16), 0.5,  {0,1});
	anim_activated = anim_def(sheet, vec2i(18, 16), 0.5,  {2,3});
	anim_respawn   = anim_def(sheet, vec2i(18, 16), 0.3,  {0,4});

	sound_activated = sound_source("assets/sounds/respawn-activate.qoa");
}

static void init(entity_t *self) {
	self->check_against = ENTITY_GROUP_PLAYER;
	self->anim = anim(anim_idle);
	self->size = vec2(18, 16);
	self->draw_order = -1;
}

static void update(entity_t *self) {
	if (self->anim.def == anim_respawn && anim_looped(&self->anim) > 4) {
		self->anim = anim(anim_activated);
	}
	entity_base_update(self);
}

static void touch(entity_t *self, entity_t *other) {
	if (self->respawn_pod.activated) {
		return;
	}
	self->respawn_pod.activated = true;
	self->anim = anim(anim_activated);
	game_set_checkpoint(entity_ref(self));
	sound_play(sound_activated);
}

static void message(entity_t *self, entity_message_t message, void *data) {
	if (message == EM_ACTIVATE) {
		self->anim = anim(anim_respawn);
	}
}

entity_vtab_t entity_vtab_respawn_pod = {
	.load = load,
	.init = init,
	.update = update,
	.touch = touch,
	.message = message,
};