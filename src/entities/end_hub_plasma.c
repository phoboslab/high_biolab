#include "../main.h"

EDITOR_IGNORE(true);

static image_t *img_plasma;

static void load(void) {
	img_plasma = image("assets/sprites/plasma.qoi");
}

void draw(entity_t *self, vec2_t viewport) {
	self->end_hub_plasma.time += engine.tick;
	self->draw_order = 8;
	float d = self->end_hub_plasma.time;
	float t = d * 100 + 16000;
	float i = self->end_hub_plasma.index;
	float xn1 = noise_gen(g.noise, vec2(i * (10.0/97.0), t * (1.0/883.0)));
	float xn2 = noise_gen(g.noise, vec2(i * (10.0/41.0), t * (1.0/311.0))) * 2.0;
	float xn3 = noise_gen(g.noise, vec2(i * (10.0/13.0), t * (1.0/89.0))) * 0.5;
	float yn1 = noise_gen(g.noise, vec2(i * (10.0/97.0), t * (1.0/701.0)));
	float yn2 = noise_gen(g.noise, vec2(i * (10.0/41.0), t * (1.0/373.0))) * 2.0;
	float yn3 = noise_gen(g.noise, vec2(i * (10.0/13.0), t * (1.0/97.0))) * 0.5;
	
	float spread = clamp(80.0 / (d * d * 0.7), 0, 1000);
	vec2_t pos = vec2(self->pos.x + (xn1+xn2+xn3) * 40 * spread, self->pos.y + (yn1+yn2+yn3) * 30 * spread);

	render_set_blend_mode(RENDER_BLEND_LIGHTER);
	image_draw(img_plasma, vec2_sub(pos, viewport));
	render_set_blend_mode(RENDER_BLEND_NORMAL);
}

entity_vtab_t entity_vtab_end_hub_plasma = {
	.load = load,
	.draw = draw
};
