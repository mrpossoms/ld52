#include "renderer.hpp"
#include "gameplay.hpp"

using mat4 = xmath::mat<4,4>;

game::Renderer::Renderer(g::asset::store& assets, game::State& state) : assets(assets), text(assets.font("UbuntuMono-B.ttf"))
{
	this->tweaker = state.tweaker;

	plane = g::gfx::mesh_factory::plane();

	camera.position = { 0, 0, 5 };
	camera.orientation = xmath::quat<>{};
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
		}
	}
}

void game::Renderer::draw(game::State& state, float dt)
{
	glDisable(GL_CULL_FACE);

	if (!state.player.done())
	{
		auto error = state.player.position - camera.position;
		error[2] = 0;
		camera_velocity += error * 0.01f * dt;
		
		camera.position[0] = state.player.position[0]; //+= error * 0.1f + camera_velocity * dt;
		camera.position[1] = state.player.position[1];	
	}

	camera.aspect_ratio(g::gfx::aspect());
	// camera.look_at(state.player.position);

	// sky
	glDisable(GL_DEPTH_TEST);
	plane.using_shader(assets.shader("post.vs+sky.fs"))
	.set_camera(camera)
	.draw<GL_TRIANGLE_FAN>();
	glEnable(GL_DEPTH_TEST);

	{ // title

        
		std::string str = "";

		if (!state.player.thrusted)
		{
			str = "Up arrow key to fire engine";
		}
		else if (!state.player.rolled)
		{
			str = "Left/right arrow key to roll";
		}
		else if (!state.player.hoovered)
		{
			str = "Space key to abduct";
		}
        
        vec<2> dims, offset;
        text.measure(str, dims, offset);
        offset = (dims * -0.5);// - offset * 0.5;

        auto s = 0.05f;
        auto model = mat4::translation({offset[0] * s, offset[1] * s, -10}) * mat4::scale({-s, s, 1.0f});

        text.draw(assets.shader("basic_gui.vs+basic_font.fs"), str, camera, model);
        // debug::print{&cam}.color({0, 1, 0, 1}).model(model).ray(vec<2>{}, dims);
        // debug::print{&cam}.color({0, 1, 0, 1}).model(model).point(offset + dims);
        // 

	}

	{ // terrain (background)
		glDisable(GL_DEPTH_TEST);
		auto& world_settings = tweaker->objects["world"];

		auto model = mat4::translation(vec<3>{camera.position[0], camera.position[1], -5}) * mat4::scale({ 100, 100, 100 });
		plane.using_shader(assets.shader("terrain-post.vs+terrain-post.fs"))
			["u_model"].mat4(model)
			["u_under_ground"].texture(world_settings.texture("ground"))
			["u_above_ground"].texture(world_settings.texture("ground"))
			.set_camera(camera)
			.draw<GL_TRIANGLE_FAN>();
		glEnable(GL_DEPTH_TEST);
	}


	glDisable(GL_DEPTH_TEST);

	draw_props(state, camera);

    for (auto& a : state.abductees)
    {

    	if (abs(a.position[0] - state.player.position[0]) > 10)
    	{
    		continue;
    	}

		auto model = mat4::translation(a.position) * mat4::rotation({0, 0, 1}, a.angle) * mat4::scale({ a.velocity[0] > 0 ? 1.f : -1.f, 1.f, 1.f});
		

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

		auto model = mat4::translation(camera.position * vec<3>{1.f, 1.f, 0.f}) * mat4::scale({10, 10, 10});
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

		state.player.sprite.update(std::min(5.f, state.player.velocity.magnitude() + state.player.thrust) * dt, 0);

		plane.using_shader(assets.shader("spritesheet.vs+spritesheet.fs"))
		["u_model"].mat4(model)
		.set_sprite(state.player.sprite)
		.set_camera(camera)
		.draw<GL_TRIANGLE_FAN>();
	}

	{ // UI
		glDisable(GL_DEPTH_TEST);
		const vec<4> red = {1, 0, 0, 1};
		const vec<4> green = {0, 1, 0, 1};

		auto p = state.player.energy / 100.f;
		auto color = green * p + red * (1.f - p);

		auto half = Abductee::Type::COUNT / 2.f;
		for (unsigned i = 0; i < Abductee::Type::COUNT; i++)
		{
			auto p = camera.position + vec<3>{i - half, 3.0f, -camera.position[2]};
			std::string str = std::to_string(state.player.abductee_counts[i]) + "/" + std::to_string(state.player.abductee_targets[i]);
		    auto s = 0.005f;

		    vec<2> dims, offset;
		    text.measure(str, dims, offset);
		    offset = (dims * -0.5) * s;// - offset * 0.5;
		    auto text_model = mat4::translation(p + vec<3>{offset[0], offset[1], 0}) * mat4::scale({-s, s, 1.0f});

		    text.using_shader(assets.shader("basic_gui.vs+basic_font.fs"), str, [&](g::gfx::shader::usage& usage) {
		    	vec<4> color = {1, 1, 1, 1};
		    	if (state.player.abductee_counts[i] > state.player.abductee_targets[i])
		    	{
		    		color = {1, 0, 0, 1};
		    	}
		    	else if (state.player.abductee_counts[i] == state.player.abductee_targets[i])
		    	{
		    		color = {0, 1, 0, 1};
		    	}

				usage.set_camera(camera)
	              ["u_model"].mat4(mat<4, 4>::translation({ 0, 0.5, 0 }) * text_model)
	              ["u_font_color"].vec4(color)
	              ["u_texture"].texture(text.font.face);
			}).draw<GL_TRIANGLES>();

			auto icon_model = mat4::translation(p);

			auto& abductee_settings = state.tweaker->objects[game::Abductee::obj_name_for_type((Abductee::Type)i)];
			auto sprite = abductee_settings.sprite("sprite").make_instance();

			plane.using_shader(assets.shader("spritesheet.vs+spritesheet.fs"))
			["u_model"].mat4(icon_model)
			.set_sprite(sprite)
			.set_camera(camera)
			.draw<GL_TRIANGLE_FAN>();				
		}

		for (int i = 0; i < 3; i++)
		{
			constexpr auto space = 0.025f;
			g::gfx::debug::print{ camera }.color({ 0.25, 0.25, 0.25, 1 }).ray(camera.position + vec<3>{0, 2 + i * space, -camera.position[2]}, { 1.f, 0, 0 });
			g::gfx::debug::print{ camera }.color({ 0.25, 0.25, 0.25, 1 }).ray(camera.position + vec<3>{0, 2 + i * space, -camera.position[2]}, { -1.f, 0, 0 });

			g::gfx::debug::print{ camera }.color(color).ray(camera.position + vec<3>{0, 2 + i * space, -camera.position[2]}, { state.player.energy / 100.f, 0, 0 });
			g::gfx::debug::print{ camera }.color(color).ray(camera.position + vec<3>{0, 2 + i * space, -camera.position[2]}, { -state.player.energy / 100.f, 0, 0 });			
		}
		glEnable(GL_DEPTH_TEST);

		
	}


}