#include "../main.h"

EDITOR_SIZE(24, 8);
EDITOR_RESIZE(false);
EDITOR_COLOR(81, 132, 188);

static anim_def_t *anim_idle;

static void load(void) {
	image_t *sheet = image("assets/sprites/elevator.qoi");

	anim_idle   = anim_def(sheet, vec2i(24, 8), 1.0, {0});
}

static void init(entity_t *self) {
	self->anim = anim(anim_idle);
	self->size = vec2(24, 8);
	self->physics = ENTITY_PHYSICS_FIXED;
	self->gravity = 0;
	self->mover.speed = 20;
}

static void settings(entity_t *self, json_t *settings) {
	self->mover.targets = entities_from_json_names(json_value_for_key(settings, "target"));
	self->mover.speed = json_number(json_value_for_key(settings, "speed"));
	if (self->mover.speed == 0) {
		self->mover.speed = 20;
	}
}

static void update(entity_t *self) {
	if (self->mover.targets.len == 0) {
		return;
	}

	float prev_distance = 0;
	entity_t *target = entity_by_ref(self->mover.targets.entities[self->mover.current_target]);

	prev_distance = entity_dist(self, target);
	float angle = entity_angle(self, target);
	self->vel = vec2_mulf(vec2_from_angle(angle), self->mover.speed);
	
	entity_base_update(self);

	// Are we close to the target or has the distance actually increased?
	// -> Set new target
	float cur_distance = entity_dist(self, target);
	if (target && (cur_distance > prev_distance || cur_distance < 0.5)) {
		self->pos = vec2_sub(
			vec2_add(target->pos, vec2_mulf(target->size, 0.5)), 
			vec2_mulf(self->size, 0.5)
		);
		self->mover.current_target = (self->mover.current_target + 1) % self->mover.targets.len;
	}
}

entity_vtab_t entity_vtab_mover = {
	.load = load,
	.init = init,
	.settings = settings,
	.update = update,
};
