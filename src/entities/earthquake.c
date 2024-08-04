#include "../main.h"

EDITOR_SIZE(8, 8);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 229, 14);

static sound_source_t *sound_earthquake;

static void load(void) {
	sound_earthquake = sound_source("assets/sounds/earthquake.qoa");
}

static void settings(entity_t *self, json_t *settings) {
	self->earthquake.strength = json_number(json_value_for_key(settings, "strength"));
	self->earthquake.duration = json_number(json_value_for_key(settings, "duration"));
}


static void update(entity_t *self) {
	if (self->earthquake.time <= 0) {
		return;
	}

	self->earthquake.time -= engine.tick;

	float p = self->earthquake.time / self->earthquake.duration;
	float shake = self->earthquake.strength * p * 0.5;
	if (shake > 0.5) {
		engine.viewport.x += rand_float(-shake, shake);
		engine.viewport.y += rand_float(-shake, shake);
	}
}

static void trigger(entity_t *self, entity_t *other) {
	sound_play(sound_earthquake);
	self->earthquake.time = self->earthquake.duration;
}

entity_vtab_t entity_vtab_earthquake = {
	.load = load,
	.settings = settings,
	.update = update,
	.trigger = trigger,
};