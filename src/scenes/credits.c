#include "../main.h"

static char *credits = 
	"          Thanks for Playing!\n"
	"\n"
	"\n"
	"Concept Graphics & Programming\n"
	"    Dominic Szablewski\n"
	"\n"
	"Music\n"
	"    Andreas Loesch\n"
	"\n"
	"Beta Testing\n"
	"    Sebastian Gerhard\n"
	"    Benjamin Hartmann\n"
	"    Jos Hirth\n"
	"    David Jacovangelo\n"
	"    Tim Juraschka\n"
	"    Christopher Klink\n"
	"    Mike Neumann\n"
	"\n"
	"\n"
	"\n"
	"\n"
	"Made with high_impact\n"
	"github.com/phoboslab/high_impact\n";

static void update(void) {
	scene_base_update();

	if (input_pressed(A_SHOOT) || input_pressed(A_JUMP) || engine.time > 44) {
		engine_set_scene(&scene_title);
	}
}

static void draw(void) {
	scene_base_draw();

	rgba_t color = rgba(255, 255, 255, max(255 - 255 * engine.time, 0));
	vec2_t size = vec2_from_vec2i(render_size());
	render_draw(vec2(0,0), size, RENDER_NO_TEXTURE, vec2(0,0), vec2(0,0), color); 

	float scroll = (engine.time - 3.5) * 10;
	font_draw(g.font, vec2(32, size.y - scroll), credits, FONT_ALIGN_LEFT);
}

scene_t scene_credits = {
	.update = update,
	.draw = draw,
};
