#include "../main.h"

EDITOR_IGNORE(true);

static void draw(entity_t *self, vec2_t viewport) {
	self->draw_order = 64;
	self->end_hub_fade.time += engine.tick;
	rgba_t color = rgba(255, 255, 255, min(255 * self->end_hub_fade.time, 255));
	vec2_t size = vec2_from_vec2i(render_size());
	render_draw(vec2(0,0), size, RENDER_NO_TEXTURE, vec2(0,0), vec2(0,0), color); 
}

entity_vtab_t entity_vtab_end_hub_fade = {
	.draw = draw,
};
