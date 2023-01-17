#include "g.h"
#include "state.hpp"
#include "tweaker.hpp"
#include "renderer.hpp"
#include "gameplay.hpp"

struct ld52 : public g::core
{

	g::asset::store assets;
	game::State state;
	std::shared_ptr<game::Renderer> renderer;


	ld52() = default;
	~ld52() = default;

	virtual bool initialize()
	{
		::srand(time(NULL));

		state.tweaker = std::make_shared<game::Tweaker>(assets);
		renderer = std::make_shared<game::Renderer>(assets, state);

		state.world.sdf = [](const vec<3>& p) -> float {
			auto d = -p[1] + sinf(p[0] / 10) + sinf(p[0] / 3) * 0.3f + sinf(p[0] + M_PI / 2) * 0.125f - 3;

			return d;
		};

		state.player.sprite = assets.sprite("ufo.json").make_instance();

		game::gameplay::reset(state);

		// for (unsigned i = 1; i--;)
		// {
		// 	state.abductees.push_back({});
		// 	auto& a = state.abductees.back();
			
		// 	a.position[0] = 0;
		// 	a.position[1] = 1;

		// 	//while (state.world.sdf(a.position + vec<3>{0, -2, 0}) < 0) a.position[1] -= 0.1f;

		// 	a.type = (unsigned)i%4;
    // 		auto& abductee_settings = state.tweaker->objects[a.obj_name()];
		// 	a.sprite = abductee_settings.sprite("sprite").make_instance();
		// }


		return true;
	}

	virtual void update(float dt)
	{
		game::gameplay::update(state, dt);

		glClearColor(0.5, 0.5, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer->draw(state, dt);
	}

};

#ifdef __EMSCRIPTEN__
EM_JS(int, canvas_get_width, (), {
  return document.getElementById('canvas').width;
});

EM_JS(int, canvas_get_height, (), {
  return document.getElementById('canvas').height;
});
#endif

int main (int argc, const char* argv[])
{
	ld52 game;

	g::core::opts opts;

	opts.name = "ld52";
	opts.gfx.fullscreen = false;
	opts.gfx.width = 1024;
	opts.gfx.height = 768;

#ifdef __EMSCRIPTEN__
	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	opts.gfx.width = canvas_get_width();
	opts.gfx.height = canvas_get_height();
#else
	opts.gfx.fullscreen = false;
#endif

	game.start(opts);


	return 0;
}
