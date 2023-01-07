#pragma once

#include "state.hpp"
#include "tweaker.hpp"

#include <g.h>

namespace game
{
namespace gameplay
{

static void update_player(
	game::State& state, 
	std::shared_ptr<game::Tweaker> tweaker, 
	float dt)
{
	auto& player = state.player;
	auto& settings = tweaker->objects["player"];
	auto& world_settings = tweaker->objects["world"];
	auto g = std::get<float>(world_settings.traits()["gravity"]);
	auto roll_speed = std::get<float>(settings.traits()["roll_speed"]);
	auto thrust = std::get<float>(settings.traits()["thrust"]);


	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_LEFT) == GLFW_PRESS) player.roll -= roll_speed * dt;
	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_RIGHT) == GLFW_PRESS) player.roll += roll_speed * dt;
	
	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_UP) == GLFW_PRESS)
	{
		player.velocity += player.up() * thrust * dt;
	}
    
    player.velocity += {0, g * dt, 0 };
    player.position += player.velocity * dt;
}

static void update(
	game::State& state, 
	std::shared_ptr<game::Tweaker> tweaker, 
	float dt)
{
	update_player(state, tweaker, dt);


}

}
}