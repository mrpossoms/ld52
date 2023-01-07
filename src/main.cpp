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
		renderer = std::make_shared<game::Renderer>(assets, state.tweaker);

		state.player.position[1] = 4;

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
