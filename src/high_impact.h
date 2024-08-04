#ifndef HIGH_IMPACT_H
#define HIGH_IMPACT_H

// -----------------------------------------------------------------------------
// High Impact configuration

// These defines are ALL optional. They overwrite the defaults set by 
// high_impact and configure aspects of the library

// The values here (particularly resource limits) have been dialed in to this
// particular game. Increase them as needed. Allocating a few GB and thousands
// of entities is totally fine.

#define ALLOC_SIZE (16 * 1024 * 1024)
#define ALLOC_TEMP_OBJECTS_MAX 8

#define WINDOW_TITLE "Biolab Disaster"
#define WINDOW_WIDTH (240*4)
#define WINDOW_HEIGHT (160*4)

#define GAME_VENDOR "phoboslab"
#define GAME_NAME "biolab"

#define RENDER_WIDTH 240
#define RENDER_HEIGHT 160
#define RENDER_RESIZE_MODE RENDER_RESIZE_WIDTH
#define RENDER_SCALE_MODE RENDER_SCALE_DISCRETE

#define ENTITIES_MAX 1024
#define ENTITY_MAX_SIZE 64
#define ENTITY_MIN_BOUNCE_VELOCITY 10

#define SOUND_MAX_UNCOMPRESSED_SAMPLES (64 * 1024)
#define SOUND_MAX_SOURCES 64
#define SOUND_MAX_NODES 256

#define IMAGE_MAX_SOURCES 64

#define RENDER_ATLAS_SIZE 64
#define RENDER_ATLAS_GRID 8
#define RENDER_ATLAS_BORDER 0
#define RENDER_BUFFER_CAPACITY 2048
#define RENDER_TEXTURES_MAX 64

#define ENGINE_MAX_TICK 0.1
#define ENGINE_MAX_BACKGROUND_MAPS 4

#include "../high_impact/src/alloc.h"
#include "../high_impact/src/animation.h"
#include "../high_impact/src/camera.h"
#include "../high_impact/src/entity_def.h"
#include "../high_impact/src/font.h"
#include "../high_impact/src/engine.h"
#include "../high_impact/src/image.h"
#include "../high_impact/src/input.h"
#include "../high_impact/src/map.h"
#include "../high_impact/src/noise.h"
#include "../high_impact/src/platform.h"
#include "../high_impact/src/render.h"
#include "../high_impact/src/sound.h"
#include "../high_impact/src/trace.h"
#include "../high_impact/src/types.h"
#include "../high_impact/src/utils.h"

// Careful, before we can include "../high_impact/src/entity.h", we need to set
// up our entity types.


// -----------------------------------------------------------------------------
// Entities

// This X-Macro will get called by ENTITY_DEFINE() and defines the entity_type_t
// enum, as well as the entity_vtab_t for each listed type.

// Each TYPE() must have a TYPE_NAME that can be used with e.g. 
// entity_spawn(ENTITY_TYPE_PLAYER, ...), and a SHORT_NAME that will can be used 
// for the stringified name with e.g. entity_type_by_name("player")

// Every entity is also expected to have a global entity_vtab_t with the name
// entity_vtab_SHORT_NAME, e.g. entity_vtab_t entity_vtab_player = {...};

#define ENTITY_TYPES(TYPE) \
	TYPE(ENTITY_TYPE_BLOB,           blob) \
	TYPE(ENTITY_TYPE_CRATE,          crate) \
	TYPE(ENTITY_TYPE_DEBRIS,         debris) \
	TYPE(ENTITY_TYPE_DELAY,          delay) \
	TYPE(ENTITY_TYPE_DROPPER,        dropper) \
	TYPE(ENTITY_TYPE_EARTHQUAKE,     earthquake) \
	TYPE(ENTITY_TYPE_END_HUB,        end_hub) \
	TYPE(ENTITY_TYPE_END_HUB_PLASMA, end_hub_plasma) \
	TYPE(ENTITY_TYPE_END_HUB_FADE,   end_hub_fade) \
	TYPE(ENTITY_TYPE_GLASS_DOME,     glass_dome) \
	TYPE(ENTITY_TYPE_GRUNT,          grunt) \
	TYPE(ENTITY_TYPE_HURT,           hurt) \
	TYPE(ENTITY_TYPE_LEVEL_CHANGE,   level_change) \
	TYPE(ENTITY_TYPE_MINE,           mine) \
	TYPE(ENTITY_TYPE_MOVER,          mover) \
	TYPE(ENTITY_TYPE_PARTICLE,       particle) \
	TYPE(ENTITY_TYPE_PLAYER,         player) \
	TYPE(ENTITY_TYPE_PROJECTILE,     projectile) \
	TYPE(ENTITY_TYPE_RESPAWN_POD,    respawn_pod) \
	TYPE(ENTITY_TYPE_SPIKE,          spike) \
	TYPE(ENTITY_TYPE_SPEWER,         spewer) \
	TYPE(ENTITY_TYPE_SPEWER_SHOT,    spewer_shot) \
	TYPE(ENTITY_TYPE_TEST_TUBE,      test_tube) \
	TYPE(ENTITY_TYPE_TRIGGER,        trigger) \
	TYPE(ENTITY_TYPE_VOID,           void) \


// All entity types share the same struct. Calling ENTITY_DEFINE() defines that
// struct with the fields required by high_impact and the additional fields
// specified here.

ENTITY_DEFINE(
	// Entity private data
	union {
		struct {
			bool in_jump;
			bool seen_player;
			float jump_timer;
		} blob;

		struct {
			int count;
			float duration;
			float duration_time;
			float emit_time;
		} debris;

		struct {
			entity_list_t targets;
			entity_ref_t triggered_by;
			bool fire;
			float delay;
			float delay_time;
		} delay;

		struct {
			float shoot_wait_time;
			float shoot_time;
			bool can_shoot;
		} dropper;

		struct {
			float duration;
			float strength;
			float time;
		} earthquake;

		struct {
			int stage;
		} end_hub;

		struct {
			float time;
			int index;
		} end_hub_plasma;

		struct {
			float time;
		} end_hub_fade;

		struct {
			float shoot_time;
			bool flip;
			bool seen_player;
		} grunt;

		struct {
			float damage;
		} hurt;

		struct {
			char *path;
		} level_change;

		struct {
			entity_list_t targets;
			int current_target;
			float speed;
		} mover;

		struct {
			float life_time;
			float fade_time;
		} particle;

		struct {
			float high_jump_time;
			float idle_time;
			bool flip;
			bool can_jump;
			bool is_idle;
		} player;

		struct {
			anim_def_t *anim_hit;
			bool has_hit;
			bool flip;
		} projectile;		

		struct {
			bool activated;
		} respawn_pod;

		struct {
			float shoot_wait_time;
			float shoot_time;
			bool can_shoot;
			bool flip;
		} spike;

		struct {
			float shoot_wait_time;
			float shoot_time;
			bool can_shoot;
		} spewer;

		struct {
			int bounce_count;
		} spewer_shot;

		struct {
			entity_list_t targets;
			float delay;
			float delay_time;
			bool can_fire;
		} trigger;		
	};
);


// The entity_message_t is used with the entity_message() function. You can
// extend this as you wish. 

typedef enum {
	EM_INVALID,
	EM_ACTIVATE,
} entity_message_t;

// Now that we have all the prerequisites, we can include entity.h
#include "../high_impact/src/entity.h"

#endif