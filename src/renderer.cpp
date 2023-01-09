#include "renderer.hpp"

using mat4 = xmath::mat<4,4>;

game::Renderer::Renderer(g::asset::store& assets, game::State& state) : assets(assets)
{
	this->tweaker = state.tweaker;

	plane = g::gfx::mesh_factory::plane();

	camera.position = { 0, 0, 5 };
	camera.orientation = xmath::quat<>{};

	player_sprite = assets.sprite("ufo.json").make_instance();
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

	// terrain->update(camera);
    // terrain->draw(camera, assets.shader("terrain.vs+terrain.fs"), [&](g::gfx::shader::usage& usage) {
    //     // this allows the user to set shader uniforms for each block before it's drawn
    //     usage["u_model"].mat4(mat4::I());
    // });
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