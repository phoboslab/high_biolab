#include "../main.h"

EDITOR_SIZE(8, 8);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 229, 14);

static void settings(entity_t *self, json_t *settings) {
	self->level_change.path = json_string(json_value_for_key(settings, "level"));
}

static void trigger(entity_t *self, entity_t *other) {
	game_set_level_path(self->level_change.path);
	engine_set_scene(&scene_stats);
}

entity_vtab_t entity_vtab_level_change = {
	.settings = settings,
	.trigger = trigger,
};