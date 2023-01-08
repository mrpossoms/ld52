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
		state.tweaker = std::make_shared<game::Tweaker>(assets);
		renderer = std::make_shared<game::Renderer>(assets, state);

		state.player.position[1] = 6;

		state.world.sdf = [](const vec<3>& p) -> float {
        auto d = -p[1] + sin(p[0] / 10) + sin(p[0] / 3) * 0.3f + sin(p[0] + M_PI / 2) * 0.125f - 3;
    
        if (p[2] > 1) { d = -1; }

        return d;
    };

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
