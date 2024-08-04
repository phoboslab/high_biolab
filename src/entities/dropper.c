#include "../main.h"

EDITOR_SIZE(14, 8);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 155, 32);

static anim_def_t *anim_idle;
static anim_def_t *anim_shoot;
static anim_def_t *anim_hit;
static anim_def_t *anim_shot_idle;
static anim_def_t *anim_shot_hit;
static anim_def_t *anim_gib;
static sound_source_t *sound_gib;

static void load(void) {
	image_t *sheet = image("assets/sprites/dropper.qoi");

	anim_idle  = anim_def(sheet, vec2i(16, 8), 1.0, {0});
	anim_shoot = anim_def(sheet, vec2i(16, 8), 0.2, {1,2,2,1});
	anim_hit   = anim_def(sheet, vec2i(16, 8), 0.2, {3});

	anim_shot_idle = anim_def(sheet, vec2i(8, 8), 1.0, {8});
	anim_shot_hit  = anim_def(sheet, vec2i(8, 8), 0.1, {9,10,11});

	image_t *gib_sheet = image("assets/sprites/blob-gibs.qoi");
	anim_gib  = anim_def(gib_sheet, vec2i(4, 4), 10, {0,1,2});

	sound_gib = sound_source("assets/sounds/wetgib.qoa");
}

static void init(entity_t *self) {
	self->anim = anim(anim_idle);
	self->size = vec2(14, 8);
	self->offset = vec2(1, 0);
	self->group = ENTITY_GROUP_ENEMY;
	self->check_against = ENTITY_GROUP_PLAYER;
	self->health = 80;
	self->dropper.shoot_wait_time = 1;
	self->dropper.shoot_time = 10;
	self->dropper.can_shoot = false;
}

static void update(entity_t *self) {
	entity_t *player = entity_by_ref(g.player);

	self->dropper.shoot_wait_time -= engine.tick;
	self->dropper.shoot_time -= engine.tick;

	if (self->anim.def == anim_hit && anim_looped(&self->anim)) {
		self->anim.def = anim_idle;
		self->dropper.shoot_wait_time = 0.5;
	}
	
	else if (
		self->anim.def == anim_idle &&
		self->dropper.shoot_wait_time < 0 &&
		vec2_dist(self->pos, player->pos) < 128
	) {
		self->anim = anim(anim_shoot);
		self->dropper.shoot_time = 0.8;
		self->dropper.can_shoot = true;
	}
	
	else if (
		self->anim.def == anim_shoot &&
		self->dropper.can_shoot &&
		self->dropper.shoot_time < 0
	) {
		self->dropper.can_shoot = false;
		vec2_t drop_pos = vec2_add(self->pos, vec2(5, 6));
		entity_t *drop = entity_spawn(ENTITY_TYPE_PROJECTILE, drop_pos);
		if (drop) {
			drop->size = vec2(4, 4);
			drop->gravity = 1;
			drop->offset = vec2(2, 4);
			drop->check_against = ENTITY_GROUP_PLAYER | ENTITY_GROUP_BREAKABLE;
			drop->anim = anim(anim_shot_idle);
			drop->projectile.anim_hit = anim_shot_hit;
			drop->vel = vec2(0, 0);
		}
	}
	
	if (self->anim.def == anim_shoot && anim_looped(&self->anim)) {
		self->anim = anim(anim_idle);
		self->dropper.shoot_wait_time = 0.5;
	}

	entity_base_update(self);
}

static void damage(entity_t *self, entity_t *other, float damage) {
	bool flip_x = self->anim.flip_x;
	self->anim = anim(anim_hit);
	self->blob.seen_player = true;
	self->blob.in_jump = false;
	self->vel.x = other->vel.x > 0 ? 50 : -50;
	
	int gib_count = self->health <= damage ? 20 : 3;
	for (int i = 0; i < gib_count; i++) {
		game_spawn_particle(self->pos, 120, 30, vec2_to_angle(other->vel), M_PI/4, anim_gib);
	}
	
	sound_play(sound_gib);
	entity_base_damage(self, other, damage);
}

static void touch(entity_t *self, entity_t *other) {
	entity_damage(other, self, 10);
}

entity_vtab_t entity_vtab_dropper = {
	.load = load,
	.init = init,
	.update = update,
	.damage = damage,
	.touch = touch,
};
