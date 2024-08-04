#include "../main.h"

EDITOR_SIZE(16, 9);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 155, 32);

static anim_def_t *anim_crawl;
static anim_def_t *anim_shoot;
static anim_def_t *anim_hit;

static anim_def_t *anim_gib;
static anim_def_t *anim_shot_idle;
static anim_def_t *anim_shot_hit;

static sound_source_t *sound_gib;

static void load(void) {
	image_t *sheet = image("assets/sprites/spike.qoi");

	anim_crawl = anim_def(sheet, vec2i(16, 16), 0.08, {5,6,7});
	anim_shoot = anim_def(sheet, vec2i(16, 16), 0.15, {3,3,0,1,2,2,2,1,3,3,3});
	anim_hit   = anim_def(sheet, vec2i(16, 16), 0.1, {8});

	anim_gib = anim_def(sheet, vec2i(8, 8), 5.0, {28,29,38,39});

	anim_shot_idle = anim_def(sheet, vec2i(8, 8), 0.1, {8, 9});
	anim_shot_hit  = anim_def(sheet, vec2i(8, 8), 0.1, {18,19});

	sound_gib = sound_source("assets/sounds/drygib.qoa");
}

static void init(entity_t *self) {
	self->group = ENTITY_GROUP_ENEMY;
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(16, 9);
	self->offset = vec2(0, 7);
	self->health = 30;
	self->anim = anim(anim_crawl);
	self->physics = ENTITY_PHYSICS_PASSIVE;
	self->spike.shoot_wait_time = 10;
}

static void shoot(vec2_t pos, vec2_t vel) {
	entity_t *spike = entity_spawn(ENTITY_TYPE_PROJECTILE, pos);
	if (spike) {
		spike->size = vec2(4, 4);
		spike->offset = vec2(2, 2);
		spike->check_against = ENTITY_GROUP_PLAYER | ENTITY_GROUP_BREAKABLE;
		spike->anim = anim(anim_shot_idle);
		spike->projectile.anim_hit = anim_shot_hit;
		spike->vel = vel;
	}
}

static void update(entity_t *self) {
	entity_t *player = entity_by_ref(g.player);

	// shooting
	if (self->anim.def == anim_shoot) {
		
		// end shooting
		if (anim_looped(&self->anim)) {
			self->anim = anim(anim_crawl);
		}
		
		// spawn spikes
		self->spike.shoot_time -= engine.tick;
		if (self->spike.can_shoot && self->spike.shoot_time < 0) {
			self->spike.can_shoot = false;
			shoot(vec2_add(self->pos, vec2( 6, -4)), vec2(  0, -60));
			shoot(vec2_add(self->pos, vec2( 1, -2)), vec2(-60,   0));
			shoot(vec2_add(self->pos, vec2(10, -2)), vec2( 60,   0));
		}
	}
	
	// Crawling
	else if (self->anim.def == anim_crawl) {
		self->spike.shoot_wait_time -= engine.tick;
		// Init shoot
		if(
			self->spike.shoot_wait_time < 0 &&
			vec2_dist(player->pos, self->pos) < 160
		) {
			self->anim = anim(anim_shoot);
			self->spike.shoot_wait_time = 5;
			self->spike.shoot_time = 1.2;
			self->spike.can_shoot = true;
			self->vel.x = 0;
		}
		
		// Crawl on
		else {
			vec2_t check_pos = vec2(
				self->pos.x + (self->spike.flip ? 4 : self->size.x - 4),
				self->pos.y + self->size.y + 1
			);

			// Near abyss? return!
			if (map_tile_at_px(engine.collision_map, check_pos) == 0) {
				self->spike.flip = !self->spike.flip;
			}
			self->vel.x = self->spike.flip ? -14 : 14;
		}
	}
	
	// Hit anim finished?
	else if (self->anim.def == anim_hit && anim_looped(&self->anim)) {
		self->anim = anim(anim_crawl);
	}

	entity_base_update(self);
}

static void damage(entity_t *self, entity_t *other, float damage) {
	self->anim = anim(anim_hit);
	self->vel.x = other->vel.x > 0 ? 50 : -50;
	
	int gib_count = self->health <= damage ? 15 : 3;
	for (int i = 0; i < gib_count; i++) {
		entity_t *particle = game_spawn_particle(entity_center(self), 120, 30, vec2_to_angle(other->vel), M_PI/4, anim_gib);
		if (particle) {
			particle->offset = vec2(2, 2);
		}
	}
	
	sound_play(sound_gib);
	entity_base_damage(self, other, damage);
}

static void collide(entity_t *self, vec2_t normal, trace_t *trace) {
	if (normal.x != 0) {
		self->spike.flip = !self->spike.flip;
	}
}

static void touch(entity_t *self, entity_t *other) {
	entity_damage(other, self, 10);
}

entity_vtab_t entity_vtab_spike = {
	.load = load,
	.init = init,
	.update = update,
	.damage = damage,
	.collide = collide,
	.touch = touch,
};