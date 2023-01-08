#include "gameplay.hpp"


static void update_player(game::State& state, float dt)
{
	auto& player = state.player;
	auto& player_settings = state.tweaker->objects["player"];
	auto& world_settings = state.tweaker->objects["world"];
	auto roll_speed = std::get<float>(player_settings.traits()["roll_speed"]);
	auto thrust = std::get<float>(player_settings.traits()["thrust"]);
	auto tractor = std::get<float>(player_settings.traits()["tractor"]);


	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_LEFT) == GLFW_PRESS) player.roll -= roll_speed * dt;
	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_RIGHT) == GLFW_PRESS) player.roll += roll_speed * dt;
	
	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_UP) == GLFW_PRESS)
	{
		player.velocity += player.up() * thrust * dt;
	}

	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		auto down = -player.up();

		for (auto& a : state.abductees)
		{
			auto delta = player.position - a.position;

			tractor *= acosf(down.dot(-delta) / (delta.magnitude())) < (10.f * M_PI / 180.f);

			a.velocity += delta * tractor * dt;
		}
	}
}

static void update_dynamics(game::State& state, float dt)
{

	auto& world_settings = state.tweaker->objects["world"];
	auto g = std::get<float>(world_settings.traits()["gravity"]);
	g::dyn::cd::sdf_collider world_collider(state.world.sdf);

	auto gravity_vec = vec<3>{0, g * dt, 0 };

	// update object velocities
	{
		state.player.velocity += gravity_vec;

		for (auto& a : state.abductees)
		{
			a.velocity += gravity_vec;
		}
	}

	// resolve collisions
	{
		// player
		auto intersections = world_collider.intersections(state.player);
		for (auto& i : intersections) { i.normal *= -1.f; }
	    g::dyn::cr::resolve_linear<game::Player>(state.player, intersections, 0.0f);
	

		// abductees
		for (auto& a : state.abductees)
		{
			auto intersections = world_collider.intersections(a);
			for (auto& i : intersections) { i.normal *= -1.f; }
			g::dyn::cr::resolve_linear<game::Abductee>(a, intersections, 0.0f);

			if ((a.position - state.player.position).magnitude() < 1)
			{
				a = state.abductees[state.abductees.size() - 1];
				state.abductees.pop_back();
			}
		}
	}

	// update positions
	{
		state.player.dyn_step(dt);
		state.player.position[2] = 0.1f; // lock player at z 0.1

		for (auto& a : state.abductees)
		{
			a.dyn_step(dt);
			a.position[2] = 0; // lock abductees at z 0
		}
	}
	
}

void game::gameplay::update(game::State& state, float dt)
{
	update_player(state, dt);

	update_dynamics(state, dt);
}