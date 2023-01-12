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

void game::Renderer::draw_suitability_grid(const game::State& state, g::game::camera& cam)
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
		auto p = vec<3>{floor(x - h_N + state.player.position[0]), ys[x], 0};

		if (x > 0 && x < N - 1)
		{
			float divergence = (-ys[x-1]) + (2.f * ys[x]) + (-ys[x+1]);

			if (divergence > 0)
			{
				auto model = mat4::translation(p);
				plane.using_shader(assets.shader("spritesheet.vs+spritesheet.fs"))
				["u_model"].mat4(model)
				.set_sprite(assets.sprite("Tree.json").make_instance())
				.set_camera(camera)
				.draw<GL_TRIANGLE_FAN>();
				g::gfx::debug::print{cam}.color({0, 1, 0, 1}).ray(p, {0.f, 1.f, 0.f});
			}
			else
			{
				g::gfx::debug::print{cam}.color({0, 0, 1, 1}).ray(p, {0.f, 1.f, 0.f});	
			}
		}
		else
		{
			g::gfx::debug::print{cam}.color({1, 0, 0, 1}).ray(p, {0.f, 1.f, 0.f});
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

	plane.using_shader(assets.shader("post.vs+tractor-beam.fs"))
	.set_camera(camera)
	["u_ship_down"].vec2(-state.player.up().slice<2>())
	["u_ship_pos"].vec3(state.player.position)
	["u_beam_angle"].flt(10.f * M_PI / 180.f)
	["u_power"].flt(state.player.hoovering)
	["u_time"].flt(state.time)
	.draw<GL_TRIANGLE_FAN>();

	glEnable(GL_DEPTH_TEST);


	glDisable(GL_CULL_FACE);

	{
		auto model = mat4::translation(vec<3>{0, game::gameplay::surface_at_x(state, 0), 0});
		
		// plane.using_shader(assets.shader("sprite.vs+sprite.fs"))
		// ["u_model"].mat4(model)
		// ["u_tex"].texture(abductee_settings.texture("sprite"))
		// .set_camera(camera)
		// .draw<GL_TRIANGLE_FAN>();

		plane.using_shader(assets.shader("spritesheet.vs+spritesheet.fs"))
		["u_model"].mat4(model)
		.set_sprite(assets.sprite("Tree.json").make_instance())
		.set_camera(camera)
		.draw<GL_TRIANGLE_FAN>();
	}

	{
		auto& world_settings = tweaker->objects["world"];

		auto model = mat4::translation(state.player.position) * mat4::scale({10, 10, 10});
		glDisable(GL_DEPTH_TEST);
		plane.using_shader(assets.shader("terrain-post.vs+terrain-post.fs"))
		["u_model"].mat4(model)
		["u_under_ground"].texture(world_settings.texture("underground"))
		["u_above_ground"].texture(world_settings.texture("ground"))
		.set_camera(camera)
		.draw<GL_TRIANGLE_FAN>();

		draw_suitability_grid(state, camera);
		glEnable(GL_DEPTH_TEST);		
	}

	glDisable(GL_DEPTH_TEST);
    for (auto& a : state.abductees)
    {

		auto model = mat4::translation(a.position);
		
		// plane.using_shader(assets.shader("sprite.vs+sprite.fs"))
		// ["u_model"].mat4(model)
		// ["u_tex"].texture(abductee_settings.texture("sprite"))
		// .set_camera(camera)
		// .draw<GL_TRIANGLE_FAN>();

		plane.using_shader(assets.shader("spritesheet.vs+spritesheet.fs"))
		["u_model"].mat4(model)
		.set_sprite(a.sprite)
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