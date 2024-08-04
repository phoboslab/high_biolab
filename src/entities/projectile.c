#include "../main.h"

EDITOR_IGNORE(true)

static void init(entity_t *self) {
	self->size = vec2(6, 3);
	self->offset = vec2(1, 2);
	self->gravity = 0;
	self->physics = ENTITY_PHYSICS_WORLD;
}

static void set_hit(entity_t *self) {
	self->projectile.has_hit = true;
	self->vel.x = 0;

	if (self->projectile.anim_hit) {
		self->anim = anim(self->projectile.anim_hit);
		self->anim.flip_x = self->projectile.flip;
	}
}

static void update(entity_t *self) {
	entity_base_update(self);

	if (self->projectile.has_hit && anim_looped(&self->anim)) {
		entity_kill(self);
	}
}

static void collide(entity_t *self, vec2_t normal, trace_t *trace) {
	if (self->projectile.has_hit) {
		return;
	}
	set_hit(self);
}


static void touch(entity_t *self, entity_t *other) {
	if (self->projectile.has_hit) {
		return;
	}
	entity_damage(other, self, 10);
	set_hit(self);
}

entity_vtab_t entity_vtab_projectile = {
	.init = init,
	.update = update,
	.collide = collide,
	.touch = touch
};