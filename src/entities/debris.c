#include "../main.h"

EDITOR_SIZE(16, 16);
EDITOR_RESIZE(true);
EDITOR_COLOR(255, 229, 14);

static anim_def_t *anim_idle;

static void load(void) {
	image_t *sheet = image("assets/sprites/debris.qoi");
	anim_idle = anim_def(sheet, vec2i(4, 4), 5.0, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14});
}

static void settings(entity_t *self, json_t *settings) {
	self->debris.count = json_number(json_value_for_key(settings, "count"));
	self->debris.duration = json_number(json_value_for_key(settings, "duration"));
	self->size.x = json_number(json_value_for_key(json_value_for_key(settings, "size"), "x"));
	self->size.y = json_number(json_value_for_key(json_value_for_key(settings, "size"), "y"));
}

static void trigger(entity_t *self, entity_t *other) {
	self->debris.duration_time = self->debris.duration;
}

static void update(entity_t *self) {
	if (self->debris.duration_time <= 0) {
		return;
	}

	self->debris.emit_time -= engine.tick;
	self->debris.duration_time -= engine.tick;
	if (self->debris.emit_time < 0) {
		self->debris.emit_time = self->debris.duration / self->debris.count;

		vec2_t spawn_pos = vec2(
			rand_float(self->pos.x, self->pos.x + self->size.x),
			rand_float(self->pos.y, self->pos.y + self->size.y)
		);
		entity_t *particle = game_spawn_particle(spawn_pos, 30, 10, 0, M_PI, anim_idle);
		if (particle) {
			particle->particle.life_time = 2;
			particle->particle.fade_time = 1;
			particle->restitution = 0.6;
		}		
	}
}

entity_vtab_t entity_vtab_debris = {
	.load = load,
	.settings = settings,
	.trigger = trigger,
	.update = update,
};