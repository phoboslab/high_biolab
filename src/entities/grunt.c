#include "../main.h"

EDITOR_SIZE(10, 13);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 155, 32);

static anim_def_t *anim_idle;
static anim_def_t *anim_walk;
static anim_def_t *anim_shoot;
static anim_def_t *anim_hit;

static anim_def_t *anim_gib;
static anim_def_t *anim_gun;
static anim_def_t *anim_shot_idle;
static anim_def_t *anim_shot_hit;

static sound_source_t *sound_gib;
static sound_source_t *sound_shoot;

static void load(void) {
	image_t *sheet = image("assets/sprites/grunt.qoi");

	anim_idle  = anim_def(sheet, vec2i(16, 16), 0.5, {0,0,0,0,1,0,0,0,1,1,1});
	anim_walk  = anim_def(sheet, vec2i(16, 16), 0.1, {6,7,8,9,10,11});
	anim_shoot = anim_def(sheet, vec2i(16, 16), 0.2, {2});
	anim_hit   = anim_def(sheet, vec2i(16, 16), 0.1, {3});

	anim_gib   = anim_def(sheet, vec2i(4, 4), 5.0, {16,17,40,41});
	anim_gun   = anim_def(sheet, vec2i(8, 8), 1.0, {11});

	image_t *projectile_sheet = image("assets/sprites/grunt-projectile.qoi");
	anim_shot_idle = anim_def(projectile_sheet, vec2i(8, 8), 1.0, {0});
	anim_shot_hit  = anim_def(projectile_sheet, vec2i(8, 8), 0.1, {0,1,2,3,4,5});

	sound_gib = sound_source("assets/sounds/drygib.qoa");
	sound_shoot = sound_source("assets/sounds/grunt-plasma.qoa");
}

static void init(entity_t *self) {
	self->group = ENTITY_GROUP_ENEMY;
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(10, 13);
	self->offset = vec2(3, 3);
	self->friction = vec2(6, 0);
	self->health = 20;
	self->anim = anim(anim_idle);
	self->grunt.flip = rand_int(0, 1);
	self->physics = ENTITY_PHYSICS_PASSIVE;
}

static void update(entity_t *self) {
	entity_t *player = entity_by_ref(g.player);
	vec2_t player_dist = vec2_abs(vec2_sub(player->pos, self->pos));
	float player_dir = player->pos.x - self->pos.x < 0 ? -1 : 1;

	self->grunt.shoot_time -= engine.tick;
	if (!self->grunt.seen_player) {
		if (player_dist.x < 160 && player_dist.y < 32) {
			self->grunt.seen_player = true;
			self->grunt.shoot_time = 1.5;
		}
	}
	else if (self->on_ground && self->anim.def != anim_hit) {
		if(
			(player_dist.x > 160 || (player_dist.x > 96 && self->anim.def == anim_walk)) &&
			self->grunt.shoot_time < 0
		) {
			if (self->anim.def != anim_walk) {
				self->anim = anim(anim_walk);
			}
			self->vel.x = 30 * player_dir;
		}
		else if (self->anim.def == anim_walk) {
			self->anim = anim(anim_idle);
			self->vel.x = 0;
			self->grunt.shoot_time = 1.0;
		}
		else if (player_dist.y < 64 && self->grunt.shoot_time < 0) {
			vec2_t plasma_pos = vec2(
				self->pos.x + (self->grunt.flip ? -3 : 5),
				self->pos.y + 6
			);
			
			entity_t *plasma = entity_spawn(ENTITY_TYPE_PROJECTILE, plasma_pos);
			if (plasma) {
				plasma->check_against = ENTITY_GROUP_PLAYER | ENTITY_GROUP_BREAKABLE;
				plasma->anim = anim(anim_shot_idle);
				plasma->projectile.flip = self->grunt.flip;
				plasma->anim.flip_x = self->grunt.flip;
				plasma->projectile.anim_hit = anim_shot_hit;
				plasma->vel = vec2(self->grunt.flip ? -120 : 120, 0);
			}

			sound_play(sound_shoot);
			self->grunt.shoot_time = 2.0;

			if (self->anim.def != anim_idle) {
				self->anim = anim(anim_idle);
			}
		}
		
		if (self->anim.def == anim_idle && self->grunt.shoot_time < 0.5) {
			self->anim = anim(anim_shoot);
		}
		
		self->grunt.flip = (player_dir < 0);
	}

	if (self->anim.def == anim_hit && anim_looped(&self->anim)) {
		self->anim = anim(anim_idle);
	}

	entity_base_update(self);

	self->anim.flip_x = self->grunt.flip;
	self->anim.tile_offset = self->grunt.flip ? 12 : 0;
}

static void damage(entity_t *self, entity_t *other, float damage) {
	self->anim = anim(anim_hit);
	self->vel.x = other->vel.x > 0 ? 60 : -60;
	if (self->grunt.shoot_time > -0.3) {
		self->grunt.shoot_time = 0.7;
	}
	self->grunt.seen_player = true;
	
	int gib_count = self->health <= damage ? 10 : 3;
	for (int i = 0; i < gib_count; i++) {
		game_spawn_particle(self->pos, 120, 30, vec2_to_angle(other->vel), M_PI/4, anim_gib);
	}

	if (self->health <= damage) {
		game_spawn_particle(self->pos, 60, 10, vec2_to_angle(other->vel), M_PI/4, anim_gun);
	}
	
	sound_play(sound_gib);
	entity_base_damage(self, other, damage);
}

static void touch(entity_t *self, entity_t *other) {
	entity_damage(other, self, 10);
}

entity_vtab_t entity_vtab_grunt = {
	.load = load,
	.init = init,
	.update = update,
	.damage = damage,
	.touch = touch
};