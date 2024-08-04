#include "../main.h"

EDITOR_IGNORE(true)

static void init(entity_t *self) {
	self->size = vec2(4, 4);
	self->offset = vec2(0, 0);
	self->physics = ENTITY_PHYSICS_LITE;
	self->friction = vec2(0.25, 0);
	self->restitution = rand_float(0.5,0.8);
	self->particle.life_time = rand_float(2.0, 5.0);
	self->particle.fade_time = 6;
}

static void update(entity_t *self) {
	self->particle.life_time -= engine.tick;

	if (self->particle.life_time < 0) {
		entity_kill(self);
		return;
	}
	self->anim.color.a = clamp(
		scale(
			self->particle.life_time,
			0, self->particle.fade_time,
			0, 255
		),
		0, 255
	);
	entity_base_update(self);
}

entity_vtab_t entity_vtab_particle = {
	.init = init,
	.update = update,
};