#include "../main.h"

EDITOR_SIZE(8, 8);
EDITOR_RESIZE(true);
EDITOR_COLOR(255, 229, 14);

static void settings(entity_t *self, json_t *settings) {
	self->trigger.targets = entities_from_json_names(json_value_for_key(settings, "target"));
	self->size.x = json_number(json_value_for_key(json_value_for_key(settings, "size"), "x"));
	self->size.y = json_number(json_value_for_key(json_value_for_key(settings, "size"), "y"));
	json_t *delay = json_value_for_key(settings, "delay");
	self->trigger.delay = delay ? json_number(delay) : -1;
	self->trigger.can_fire = true;

	char *checks = json_string(json_value_for_key(settings, "checks"));
	if (checks) {
		if (str_contains(checks, "player"))     { self->check_against |= ENTITY_GROUP_PLAYER; }
		if (str_contains(checks, "npc"))        { self->check_against |= ENTITY_GROUP_NPC; }
		if (str_contains(checks, "enemy"))      { self->check_against |= ENTITY_GROUP_ENEMY; }
		if (str_contains(checks, "item"))       { self->check_against |= ENTITY_GROUP_ITEM; }
		if (str_contains(checks, "projectile")) { self->check_against |= ENTITY_GROUP_PROJECTILE; }
		if (str_contains(checks, "breakable"))  { self->check_against |= ENTITY_GROUP_BREAKABLE; }
	}
	else {
		self->check_against = ENTITY_GROUP_PLAYER;
	}
}

static void update(entity_t *self) {
	self->trigger.delay_time -= engine.tick;
}

static void touch(entity_t *self, entity_t *other) {
	if (
		!self->trigger.can_fire ||
		self->trigger.delay_time > 0
	) {
		return;
	}
	
	for (int i = 0; i < self->trigger.targets.len; i++) {
		entity_t *target = entity_by_ref(self->trigger.targets.entities[i]);
		if (target) {
			entity_trigger(target, other);
		}
	}

	if (self->trigger.delay == -1) {
		self->trigger.can_fire = false;
	}
	else {
		self->trigger.delay_time = self->trigger.delay;
	}
}

entity_vtab_t entity_vtab_trigger = {
	.settings = settings,
	.update = update,
	.touch = touch,
};