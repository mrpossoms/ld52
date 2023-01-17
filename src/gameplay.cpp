#include "gameplay.hpp"

static void update_player(game::State& state, float dt)
{
	auto& player = state.player;
	auto& player_settings = state.tweaker->objects["player"];
	auto& world_settings = state.tweaker->objects["world"];
	auto roll_speed = std::get<float>(player_settings.traits()["roll_speed"]);
	auto thrust = std::get<float>(player_settings.traits()["thrust"]);
	const auto tractor_base = std::get<float>(player_settings.traits()["tractor"]);

	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_R) == GLFW_PRESS)
	{
		game::gameplay::reset(state);
	}

	auto done = player.done();

	if (player.energy > 0 && !done)
	{
		if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			player.roll -= roll_speed * dt;
			player.rolled = player.thrusted;
		}

		if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			player.roll += roll_speed * dt;
			player.rolled = player.thrusted;
		}

		if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_UP) == GLFW_PRESS)
		{
			player.velocity += player.up() * thrust * dt;
			player.energy -= thrust * player.weight() * dt;
			player.thrust = thrust;
			player.thrusted = true;
			player.target_alt = NAN;
		}
		else
		{
			player.thrust = 0;
		}		
	}

	if (done)
	{
		player.target_alt = 10;
		player.roll = 0;
	}

	if (isfinite(player.target_alt))
	{
		float error = player.target_alt - player.position[1];
		if (error < 0) { error = 0; }

		player.velocity += player.up() * (error * 1.f) * dt;
		player.velocity *= 0.99f;
		player.thrust = 1;
		player.hoovering = 0;
	}

	if (glfwGetKey(g::gfx::GLFW_WIN, GLFW_KEY_SPACE) == GLFW_PRESS && player.energy > 0)
	{
		auto down = -player.up();

		player.hoovering = 1.0f;

		for (auto& a : state.abductees)
		{
			auto delta = player.position - a.position;

			auto tractor = tractor_base * (acosf(down.dot(-delta) / (delta.magnitude())) < (10.f * M_PI / 180.f)) / delta.dot(delta);

			player.hoovering += std::min(0.25f, tractor);

			a.velocity += delta * tractor * dt;
			a.ang_vel += tractor * 3 * dt;
		}

		player.energy -= player.hoovering * dt;
		player.hoovered = player.rolled;
	}
	else
	{
		player.hoovering = 0;
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
	

		if (intersections.size() > 0)
		{
			state.player.energy = 0;
			state.player.roll = 0;
			state.player.sprite.track("crash");
		}

		// abductees
		for (auto& a : state.abductees)
		{
	    	if (abs(a.position[0] - state.player.position[0]) > 10)
	    	{
	    		continue;
	    	}

			auto intersections = world_collider.intersections(a);
			for (auto& i : intersections) { i.normal *= -1.f; }
			
			if (intersections.size() > 0)
			{ // touching ground
				if (a.velocity[1] < -1.f)
				{ // they hit the ground with too much force and died
					a = state.abductees[state.abductees.size() - 1];
					state.abductees.pop_back();
				}
			}

			// correct velocities to avoid penetration
			g::dyn::cr::resolve_linear<game::Abductee>(a, intersections, 0.0f);

			// Despawn when they are close to the ship
			if ((a.position - state.player.position).magnitude() < 1 && state.player.hoovering > 0)
			{
				if (a.type == game::Abductee::Type::fuel)
				{
					state.player.energy = std::min(100.f, state.player.energy + 25);
				}
				else
				{
					state.player.abductee_counts[(unsigned)a.type] += 1;
				}

				a = state.abductees[state.abductees.size() - 1];
				state.abductees.pop_back();
			}

			a.sprite.update(dt, 0);

			if (intersections.size() > 0)
			{ // touching the ground
				a.velocity[0] += a.move.speed * dt;
				a.velocity[1] += 2.f * dt;
				a.ang_vel = 0;
				a.angle = 0;
			}

	    	a.move.count_down -= dt;
	    
			if (a.type != game::Abductee::Type::fuel)
			{
				if ((a.position - state.player.position).magnitude() < 4)
				{
					a.move.count_down = 1;
					a.move.speed = std::clamp(a.position[0] - state.player.position[0], -1.f, 1.f) * 20.f;
				}

				if (a.move.count_down <= 0)
				{
					a.move.next();
				}
			}
		}
	}

	// update positions
	{
		state.player.dyn_step(dt);
		//state.player.position[2] = 0.1f; // lock player at z 0.1

		for (auto& a : state.abductees)
		{
			a.dyn_step(dt);
			a.angle += a.ang_vel * dt;
			a.position[2] = 0; // lock abductees at z 0
		}
	}
	
}

void update_world(game::State& state)
{
	constexpr auto N = 15;
	constexpr auto h_N = (N - 1) >> 1;
	static float ys[N];

	int player_x = state.player.position[0];

	for (int x = 0; x < N; x++)
	{
		ys[x] = game::gameplay::surface_at_x(state, x - h_N + player_x);
	}

	for (int x = 0; x < N; x++)
	{
		// auto p = vec<3>{(float)(x - h_N + player_x), ys[x], 0};

		if (x > 0 && x < N - 1)
		{
			float divergence = (-ys[x-1]) + (2.f * ys[x]) + (-ys[x+1]);
			auto _x = x - h_N + player_x;

			if (divergence > 0)
			{
				if(!state.world.spawns.contains(_x))
				{
					auto num = ::rand() % 4;
					auto sprite_idx = std::clamp(static_cast<int>(4.f * (sin(_x) * 0.5f + 0.5f)), 0, 3);

					if (sprite_idx == 0 && ::rand() % 2 == 0)
					{
						state.abductees.push_back({});
						auto& a = state.abductees.back();
						a.type = game::Abductee::Type::fuel;
			    	
						a.position[0] = ((::rand() % 1024 / 512.f) - 1.f) + _x;
						a.position[1] = ys[x] + 0.25f;
						
						auto& abductee_settings = state.tweaker->objects[a.obj_name()];
						a.sprite = abductee_settings.sprite("sprite").make_instance();
					}

					for (;num--;)
					{
						state.abductees.push_back({});
						auto& a = state.abductees.back();
						
						a.position[0] = ((::rand() % 1024 / 512.f) - 1.f) + _x;
						a.position[1] = ys[x] + 0.25f;

						a.type = (unsigned)::rand()%game::Abductee::Type::COUNT;
			    		auto& abductee_settings = state.tweaker->objects[a.obj_name()];
						a.sprite = abductee_settings.sprite("sprite").make_instance();
						a.move.next();
					}

					state.world.spawns.insert(_x);
				}
			}
		}
	}
}

void game::gameplay::reset(game::State& state)
{
	state.world.spawns.clear();
	state.abductees.clear();
	state.time = 0;

	state.player.position[1] = 4;
	state.player.position[2] = 0.1f;
	state.player.reset();
}

float game::gameplay::surface_at_x(const game::State& state, float x)
{
	std::function<float(float, float, unsigned)> subdivide = [&](float y0, float y1, unsigned depth) -> float {
		// auto d0 = state.world.sdf({x, y0, 0});
		auto mid = (y0 + y1) * 0.5f;
		auto d = state.world.sdf({x, mid, 0});
		// auto d1 = state.world.sdf({x, y1, 0});
		
		if (depth > 7) return mid;

		if (d < 0) 
		{ // in terrain
			return subdivide(y0, mid, depth + 1);
		}
		else
		{
			return subdivide(mid, y1, depth + 1);
		}
	};

	return subdivide(-4, 4, 0);
}

void game::gameplay::update(game::State& state, float dt)
{
	update_player(state, dt);

	update_dynamics(state, dt);
	
	update_world(state);

	state.time += dt;
}