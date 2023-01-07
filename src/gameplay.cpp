#include "gameplay.hpp"


void game::gameplay::update_player(game::State& state, float dt)
{
	auto& player = state.player;
	auto& player_settings = state.tweaker->objects["player"];
	auto& world_settings = state.tweaker->objects["world"];
	auto roll_speed = std::get<float>(player_settings.traits()["roll_speed"]);
	auto thrust = std::get<float>(player_settings.traits()["thrust"]);


	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_LEFT) == GLFW_PRESS) player.roll -= roll_speed * dt;
	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_RIGHT) == GLFW_PRESS) player.roll += roll_speed * dt;
	
	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_UP) == GLFW_PRESS)
	{
		player.velocity += player.up() * thrust * dt;
	}
}

void game::gameplay::update_dynamics(game::State& state, float dt)
{
	auto world_sdf = [&](const vec<3>& p) -> float {
        return -p[1] + 3 * sin(p[0]) - 3;
    };
	auto& world_settings = state.tweaker->objects["world"];
	auto g = std::get<float>(world_settings.traits()["gravity"]);
	g::dyn::cd::sdf_collider world_collider(world_sdf);

	// update object velocities
	{
		state.player.velocity += {0, g * dt, 0 };
	}

	// resolve collisions
	{
		auto intersections = world_collider.intersections(state.player);

		for (auto& i : intersections) { i.normal *= -1.f; }

	    g::dyn::cr::resolve_linear<game::Player>(state.player, intersections, 0.0f);
	}

	// update positions
	{
		state.player.dyn_step(dt);
	}
	
}

void game::gameplay::update(game::State& state, float dt)
{
	update_player(state, dt);

	update_dynamics(state, dt);
}