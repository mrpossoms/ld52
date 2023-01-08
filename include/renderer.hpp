#pragma once
#include "state.hpp"
#include "tweaker.hpp"

#include <g.h>

namespace game
{

struct Renderer
{
	Renderer(g::asset::store& assets, game::State& state);

	void draw(game::State& state, float dt);

private:
	g::asset::store& assets;
	std::shared_ptr<game::Tweaker> tweaker;
	
	g::game::camera_perspective camera;
	vec<3> camera_velocity;

	g::gfx::mesh<g::gfx::vertex::pos_uv_norm> plane;
	std::unique_ptr<g::gfx::density_volume<g::gfx::vertex::pos_norm_tan>> terrain;
};

}