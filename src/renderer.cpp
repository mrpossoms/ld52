#include "renderer.hpp"
#include "gameplay.hpp"

using mat4 = xmath::mat<4,4>;

game::Renderer::Renderer(g::asset::store& assets, game::State& state) : assets(assets)
{
	this->tweaker = state.tweaker;

	plane = g::gfx::mesh_factory::plane();

	camera.position = { 0, 0, 5 };
	camera.orientation = xmath::quat<>{};

	player_sprite = assets.sprite("ufo.json").make_instance();
}

void game::Renderer::draw_props(const game::State& state, g::game::camera& cam)
{
	constexpr auto N = 15;
	constexpr auto h_N = (N - 1) >> 1;
	static float ys[N];

	int player_x = state.player.position[0];

	for (int x = 0; x < N; x++)
	{
		ys[x] = game::gameplay::surface_at_x(state, x - h_N + player_x);
	}

	g::gfx::sprite::instance sprites[] = {
		assets.sprite("barn.json").make_instance(),		
		assets.sprite("Rock.json").make_instance(),
		assets.sprite("haybale.json").make_instance(),
		assets.sprite("Tree.json").make_instance(),
	};

	constexpr float y_offsets[] = {
		0.4f,
		0.0625f,
		0.125f,
		0.5f,
	};

	for (int x = 0; x < N; x++)
	{
		auto sprite_idx = std::clamp(static_cast<int>(4.f * (sin(player_x + x) * 0.5f + 0.5f)), 0, 3);
		auto p = vec<3>{(float)(x - h_N + player_x), ys[x] + y_offsets[sprite_idx], 0};

		if (x > 0 && x < N - 1)
		{
			float divergence = (-ys[x-1]) + (2.f * ys[x]) + (-ys[x+1]);

			if (divergence > 0)
			{
				auto model = mat4::translation(p);
				plane.using_shader(assets.shader("spritesheet.vs+spritesheet.fs"))
				["u_model"].mat4(model)
				.set_sprite(sprites[sprite_idx])
				.set_camera(camera)
				.draw<GL_TRIANGLE_FAN>();
			}
			else
			{
				// g::gfx::debug::print{cam}.color({0, 0, 1, 1}).ray(p, {0.f, 1.f, 0.f});	
			}
		}
		else
		{
			// g::gfx::debug::print{cam}.color({1, 0, 0, 1}).ray(p, {0.f, 1.f, 0.f});
		}
	}
}

void game::Renderer::draw(game::State& state, float dt)
{
	auto error = state.player.position - camera.position;
	error[2] = 0;
	camera_velocity += error * 0.01f * dt;

	camera.position[0] = state.player.position[0]; //+= error * 0.1f + camera_velocity * dt;
	camera.position[1] = state.player.position[1];

	camera.aspect_ratio(g::gfx::aspect());
	// camera.look_at(state.player.position);

	// sky
	glDisable(GL_DEPTH_TEST);
	plane.using_shader(assets.shader("post.vs+sky.fs"))
	.set_camera(camera)
	.draw<GL_TRIANGLE_FAN>();
	glEnable(GL_DEPTH_TEST);


	glDisable(GL_CULL_FACE);

	glDisable(GL_DEPTH_TEST);

	draw_props(state, camera);

    for (auto& a : state.abductees)
    {

    	if (abs(a.position[0] - state.player.position[0]) > 10)
    	{
    		continue;
    	}

		auto model = mat4::translation(a.position) * mat4::scale({ a.velocity[0] > 0 ? 1.f : -1.f, 1.f, 1.f});
		

		plane.using_shader(assets.shader("spritesheet.vs+spritesheet.fs"))
		["u_model"].mat4(model)
		.set_sprite(a.sprite)
		.set_camera(camera)
		.draw<GL_TRIANGLE_FAN>();
    }

	{ // tractor beam
		plane.using_shader(assets.shader("post.vs+tractor-beam.fs"))
		.set_camera(camera)
		["u_ship_down"].vec2(-state.player.up().slice<2>())
		["u_ship_pos"].vec3(state.player.position)
		["u_beam_angle"].flt(10.f * M_PI / 180.f)
		["u_power"].flt(state.player.hoovering)
		["u_time"].flt(state.time)
		.draw<GL_TRIANGLE_FAN>();
	}

	{ // terrain
		auto& world_settings = tweaker->objects["world"];

		auto model = mat4::translation(state.player.position * vec<3>{1.f, 1.f, 0.f}) * mat4::scale({10, 10, 10});
		plane.using_shader(assets.shader("terrain-post.vs+terrain-post.fs"))
		["u_model"].mat4(model)
		["u_under_ground"].texture(world_settings.texture("underground"))
		["u_above_ground"].texture(world_settings.texture("ground"))
		.set_camera(camera)
		.draw<GL_TRIANGLE_FAN>();
	}

	glEnable(GL_DEPTH_TEST);

    // player
	{
		auto model = mat4::translation(state.player.position) * mat4::rotation({0, 0, 1}, state.player.roll);
		auto& player_settings = tweaker->objects["player"];

		plane.using_shader(assets.shader("spritesheet.vs+spritesheet.fs"))
		["u_model"].mat4(model)
		.set_sprite(player_sprite)
		.set_camera(camera)
		.draw<GL_TRIANGLE_FAN>();
	}
}