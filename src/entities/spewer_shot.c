#include "../main.h"

EDITOR_IGNORE(true)

static anim_def_t *anim_idle;

static void load(void) {
	image_t *sheet = image("assets/sprites/spewer.qoi");

	anim_idle = anim_def(sheet, vec2i(4, 4), 5.0, {16});
}

static void init(entity_t *self) {
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(4, 4);
	self->friction = vec2(1, 0);
	self->anim = anim(anim_idle);
	self->restitution = 0.7;
	self->physics = ENTITY_PHYSICS_LITE;
	self->check_against = ENTITY_GROUP_PLAYER;

	entity_t *player = entity_by_ref(g.player);
	float dir = self->pos.x - player->pos.x > 0 ? -1 : 1;
	self->vel.x = rand_float(40, 120) * dir;
	self->vel.y = -100;
}

static void collide(entity_t *self, vec2_t normal, trace_t *trace) {
	self->spewer_shot.bounce_count++;
	if (self->spewer_shot.bounce_count >= 3) {
		entity_kill(self);
	}
}

static void touch(entity_t *self, entity_t *other) {
	entity_damage(other, self, 10);
	entity_kill(self);
}

entity_vtab_t entity_vtab_spewer_shot = {
	.load = load,
	.init = init,
	.collide = collide,
	.touch = touch
};