#include "../main.h"

EDITOR_SIZE(16, 8);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 155, 32);

static anim_def_t *anim_idle;
static anim_def_t *anim_spew;
static anim_def_t *anim_hit;
static anim_def_t *anim_gib;

static sound_source_t *sound_gib;

static void load(void) {
	image_t *sheet = image("assets/sprites/spewer.qoi");

	anim_idle  = anim_def(sheet, vec2i(16, 8), 0.5, {0,0,0,0,0,0,0,0,1});
	anim_spew  = anim_def(sheet, vec2i(16, 8), 0.1, {1,2,2,1,1});
	anim_hit   = anim_def(sheet, vec2i(16, 8), 0.2, {3});
	anim_gib   = anim_def(sheet, vec2i(4, 4), 1.0, {16});

	sound_gib = sound_source("assets/sounds/drygib.qoa");
}

static void init(entity_t *self) {
	self->group = ENTITY_GROUP_ENEMY;
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(16, 8);
	self->offset = vec2(0, 0);
	self->health = 20;
	self->anim = anim(anim_idle);
	self->spewer.shoot_wait_time = 1;
	self->spewer.shoot_time = 10;
}

static void update(entity_t *self) {
	self->spewer.shoot_time -= engine.tick;
	self->spewer.shoot_wait_time -= engine.tick;

	entity_t *player = entity_by_ref(g.player);

	if (self->anim.def == anim_hit && anim_looped(&self->anim)) {
		self->anim = anim(anim_idle);
		self->spewer.shoot_wait_time = 0.5;
	}
	
	else if(
		self->anim.def == anim_idle &&
		self->spewer.shoot_wait_time < 0 &&
		vec2_dist(player->pos, self->pos) < 80
	) {
		self->anim = anim(anim_spew);
		self->spewer.shoot_time = 0.45;
		self->spewer.can_shoot = true;
	}
	
	else if(
		self->anim.def == anim_spew &&
		self->spewer.can_shoot &&
		self->spewer.shoot_time < 0
	) {
		self->spewer.can_shoot = false;
		vec2_t spew_pos = vec2_add(self->pos, vec2(4, 4));
		entity_spawn(ENTITY_TYPE_SPEWER_SHOT, spew_pos);
	}
	
	if (self->anim.def == anim_spew && anim_looped(&self->anim)) {
		self->anim = anim(anim_idle);
		self->spewer.shoot_wait_time = 1.5;
	}
	
	self->anim.flip_x = (self->pos.x - player->pos.x < 0);

	entity_base_update(self);
}

static void damage(entity_t *self, entity_t *other, float damage) {
	self->anim = anim(anim_hit);

	int gib_count = self->health <= damage ? 10 : 3;
	for (int i = 0; i < gib_count; i++) {
		game_spawn_particle(entity_center(self), 120, 30, vec2_to_angle(other->vel), M_PI/4, anim_gib);
	}
	
	sound_play(sound_gib);
	entity_base_damage(self, other, damage);
}

static void touch(entity_t *self, entity_t *other) {
	entity_damage(other, self, 10);
}

entity_vtab_t entity_vtab_spewer = {
	.load = load,
	.init = init,
	.update = update,
	.damage = damage,
	.touch = touch
};