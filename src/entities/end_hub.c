#include "../main.h"

EDITOR_SIZE(24, 24);
EDITOR_RESIZE(false);
EDITOR_COLOR(81, 132, 188);

static anim_def_t *anim_idle;
static anim_def_t *anim_activated;
static sound_source_t *sound_activate;
static sound_source_t *sound_the_end;

static void load(void) {
	image_t *sheet = image("assets/sprites/endhub.qoi");

	anim_idle      = anim_def(sheet, vec2i(24, 24), 0.5, {0, 1});
	anim_activated = anim_def(sheet, vec2i(24, 24), 0.5, {2, 3});

	sound_activate = sound_source("assets/sounds/respawn-activate.qoa");
	sound_the_end  = sound_source("assets/sounds/theend.qoa");
}

static void init(entity_t *self) {
	self->anim = anim(anim_idle);
	self->size = vec2(24, 24);
	self->draw_order = -1;
}

static void update(entity_t *self) {
	if (self->end_hub.stage >= 1) {
		float v = sound_volume(g.music);
		v -= engine.tick * 0.25;
		sound_set_volume(g.music, v);
	}
}

static void trigger(entity_t *self, entity_t *other) {
	// The end hub gets triggered multiple time through delays; each time we 
	// start a new stage of the animation.

	self->end_hub.stage++;

	if (self->end_hub.stage == 1) {
		sound_play(sound_activate);
		self->anim = anim(anim_activated);
	}

	// Stage 2 - plasma animation
	else if (self->end_hub.stage == 2) {
		sound_play(sound_the_end);
		vec2_t sp = vec2(self->pos.x, self->pos.y - 24);
		for (int i = 0; i < 100; i++) {
			entity_t *p = entity_spawn(ENTITY_TYPE_END_HUB_PLASMA, sp);
			if (p) {
				p->end_hub_plasma.index = i;
			}
		}
	}

	// Stage 3 - fade to white
	else if (self->end_hub.stage == 3) {
		entity_spawn(ENTITY_TYPE_END_HUB_FADE, self->pos);
	}

	// Stage 4 - end the game
	else if (self->end_hub.stage == 4) {
		engine_set_scene(&scene_credits);
	}	
}

entity_vtab_t entity_vtab_end_hub = {
	.load = load,
	.init = init,
	.update = update,
	.trigger = trigger,
};
