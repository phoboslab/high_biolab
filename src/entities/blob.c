#include "../main.h"

EDITOR_SIZE(10, 13);
EDITOR_RESIZE(false);
EDITOR_COLOR(255, 155, 32);

static anim_def_t *anim_idle;
static anim_def_t *anim_crawl;
static anim_def_t *anim_jump;
static anim_def_t *anim_hit;

static anim_def_t *anim_gib;

static sound_source_t *sound_gib;

static void load(void) {
	image_t *sheet = image("assets/sprites/blob.qoi");

	anim_idle  = anim_def(sheet, vec2i(16, 16), 0.5, {1,2,2,2,2,2,2,2,2,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2});
	anim_crawl = anim_def(sheet, vec2i(16, 16), 0.1, {0,1});
	anim_jump  = anim_def(sheet, vec2i(16, 16), 0.2, {2,3,4});
	anim_hit   = anim_def(sheet, vec2i(16, 16), 0.1, {5});

	image_t *gib_sheet = image("assets/sprites/blob-gibs.qoi");
	anim_gib  = anim_def(gib_sheet, vec2i(4, 4), 10, {0,1,2});

	sound_gib = sound_source("assets/sounds/wetgib.qoa");
}

static void init(entity_t *self) {
	self->group = ENTITY_GROUP_ENEMY;
	self->check_against = ENTITY_GROUP_PLAYER;
	self->size = vec2(10, 13);
	self->offset = vec2(3, 3);
	self->health = 20;
	self->anim = anim(anim_idle);
	anim_goto_rand(&self->anim);
	self->anim.flip_x = rand_int(0, 1);
	self->physics = ENTITY_PHYSICS_PASSIVE;
}

static void update(entity_t *self) {
	entity_t *player = entity_by_ref(g.player);
	vec2_t player_dist = vec2_abs(vec2_sub(player->pos, self->pos));
	float player_dir = player->pos.x - self->pos.x < 0 ? -1 : 1;

	if (!self->blob.seen_player) {
		if (player_dist.x < 64 && player_dist.y < 20) {
			self->blob.seen_player = true;
		}
	}
	else if (self->on_ground && self->anim.def != anim_hit) {
		self->blob.jump_timer += engine.tick;
		// Init jump
		if( 
			self->anim.def != anim_jump && self->blob.jump_timer > 0.5 && (
				(player_dist.x < 40 && player_dist.y > 20) || // near player
				map_tile_at_px(engine.collision_map, vec2_add(self->pos, vec2(self->size.x * player_dir, self->size.y))) == 1 // or blocked by wall
			)
		) {
			self->anim = anim(anim_jump);
			self->anim.flip_x = (player_dir < 0);
			self->vel.x = 0;
		}
		
		// Jump
		else if (self->anim.def == anim_jump && anim_looped(&self->anim)) {
			self->vel.y = -70;
			self->vel.x = 60 * (self->anim.flip_x ? -1 : 1);
			self->blob.in_jump = true;
		}
		
		// Crawl
		else if (self->anim.def != anim_jump && self->blob.jump_timer > 0.2) {
			self->anim = anim(anim_crawl);
			self->anim.flip_x = (player_dir < 0);
			self->vel.x = 20 * player_dir;
		}
	}

	if (self->anim.def == anim_hit && anim_looped(&self->anim)) {
		self->anim = anim(anim_idle);
	}
	
	if (self->blob.in_jump && self->vel.x == 0 && self->anim.def != anim_hit) {
		self->vel.x = 30 * (self->anim.flip_x ? -1 : 1);
	}

	bool was_on_ground = self->on_ground;

	entity_base_update(self);

	// Just landed?
	if (self->on_ground && !was_on_ground && self->anim.def != anim_hit) {
		self->blob.in_jump = false;
		self->blob.jump_timer = 0;
		bool flip_x = self->anim.flip_x;
		self->anim = anim(anim_idle);
		self->anim.flip_x = flip_x;
		self->vel.x = 0;
	}
}

static void damage(entity_t *self, entity_t *other, float damage) {
	bool flip_x = self->anim.flip_x;
	self->anim = anim(anim_hit);
	self->blob.seen_player = true;
	self->blob.in_jump = false;
	self->vel.x = other->vel.x > 0 ? 50 : -50;
	
	int gib_count = self->health <= damage ? 30 : 5;
	for (int i = 0; i < gib_count; i++) {
		game_spawn_particle(self->pos, 120, 30, vec2_to_angle(other->vel), M_PI/4, anim_gib);
	}
	
	sound_play(sound_gib);
	entity_base_damage(self, other, damage);
}

static void touch(entity_t *self, entity_t *other) {
	entity_damage(other, self, 10);
}

entity_vtab_t entity_vtab_blob = {
	.load = load,
	.init = init,
	.update = update,
	.damage = damage,
	.touch = touch
};