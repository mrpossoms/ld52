#pragma once
#include "state.hpp"
#include "tweaker.hpp"

#include <g.h>

namespace game
{

struct Renderer
{
	Renderer(g::asset::store& assets, std::shared_ptr<game::Tweaker> tweaker);

	void draw(game::State& state, float dt);

private:
	g::asset::store& assets;
	std::shared_ptr<game::Tweaker> tweaker;
	g::game::camera_perspective camera;

	g::gfx::mesh<g::gfx::vertex::pos_uv_norm> plane;
};

}