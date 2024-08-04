#include "../main.h"

EDITOR_SIZE(8, 8);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 229, 14);

static void settings(entity_t *self, json_t *settings) {
	self->delay.targets = entities_from_json_names(json_value_for_key(settings, "target"));
	self->delay.delay = json_number(json_value_for_key(settings, "delay"));
}

static void update(entity_t *self) {
	self->delay.delay_time -= engine.tick;
	if (!self->delay.fire || self->delay.delay_time > 0) {
		return;
	}
	self->delay.fire = false;

	for (int i = 0; i < self->delay.targets.len; i++) {
		entity_t *target = entity_by_ref(self->delay.targets.entities[i]);
		if (target) {
			entity_trigger(target, entity_by_ref(self->delay.triggered_by));
		}
	}
}

static void trigger(entity_t *self, entity_t *other) {
	self->delay.fire = true;
	self->delay.delay_time = self->delay.delay;
	self->delay.triggered_by = entity_ref(other);
}

entity_vtab_t entity_vtab_delay = {
	.settings = settings,
	.update = update,
	.trigger = trigger,
};