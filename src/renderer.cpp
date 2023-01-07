#include "renderer.hpp"

using mat4 = xmath::mat<4,4>;

game::Renderer::Renderer(g::asset::store& assets, std::shared_ptr<game::Tweaker> tweaker) : assets(assets)
{
	this->tweaker = tweaker;

	plane = g::gfx::mesh_factory::plane();

	camera.position = { 0, 0, 10 };
	camera.orientation = xmath::quat<>{};
}


void game::Renderer::draw(game::State& state, float dt)
{
	auto error = state.player.position - camera.position;
	error[2] = 0;
	camera_velocity += error * 0.01f * dt;

	camera.position += error * 0.1f + camera_velocity * dt;

	camera.aspect_ratio(g::gfx::aspect());
	// camera.look_at(state.player.position);

	glDisable(GL_DEPTH_TEST);
	plane.using_shader(assets.shader("sky.vs+sky.fs"))
	.set_camera(camera)
	.draw<GL_TRIANGLE_FAN>();	
	glEnable(GL_DEPTH_TEST);


	auto model = mat4::translation(state.player.position) * mat4::rotation({0, 0, 1}, state.player.roll);

	auto& player_settings = tweaker->objects["player"];

	glDisable(GL_CULL_FACE);

	plane.using_shader(assets.shader("sprite.vs+sprite.fs"))
	["u_model"].mat4(model)
	["u_tex"].texture(player_settings.texture("sprite"))
	.set_camera(camera)
	.draw<GL_TRIANGLE_FAN>();

	glDisable(GL_DEPTH_TEST);
	g::gfx::debug::print{camera}.color({1, 0, 0, 1}).ray(state.player.position, state.player.velocity);
	glEnable(GL_DEPTH_TEST);
}