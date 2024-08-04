#include "../main.h"

EDITOR_SIZE(8, 8);
EDITOR_RESIZE(true);
EDITOR_COLOR(255, 229, 14);

static void settings(entity_t *self, json_t *settings) {
	self->hurt.damage = json_number(json_value_for_key(settings, "damage"));
	if (self->hurt.damage == 0) {
		self->hurt.damage = 10;
	}
}

static void trigger(entity_t *self, entity_t *other) {
	entity_damage(other, self, self->hurt.damage);
}

entity_vtab_t entity_vtab_hurt = {
	.settings = settings,
	.trigger = trigger,
};