#include "renderer.hpp"

using mat4 = xmath::mat<4,4>;

game::Renderer::Renderer(g::asset::store& assets, game::State& state) : assets(assets)
{
	this->tweaker = state.tweaker;

	plane = g::gfx::mesh_factory::plane();

	camera.position = { 0, 0, 5 };
	camera.orientation = xmath::quat<>{};

    // function for emitting a single vertex given a position and the sdf defined above
    auto generator = [](const g::game::sdf& sdf, const vec<3>& pos) -> g::gfx::vertex::pos_norm_tan
    {
        g::gfx::vertex::pos_norm_tan v;

        const float s = 1;
        v.normal = normal_from_sdf(sdf, pos, s);
        v.position = pos;

        if (fabs(v.normal.dot({0, 1, 0})) > 0.999f)
        {
            v.tangent = vec<3>::cross(v.normal, {1, 0, 0});
        }
        else
        {
            v.tangent = vec<3>::cross(v.normal, {0, 1, 0});   
        }
    
        return v;
    };

    // define a list of integer offsets which describe the relative positioning
    // of the blocks of terrain which will be generated by the density_volume instance below
    std::vector<vec<3>> offsets;

    for (float x = -1; x <= 1; x++)
    for (float y = -1; y <= 1; y++)
    {
        offsets.push_back({x, y, 0});
    }

    // setup the density_volume instance which will be responsible for terrain generation
    terrain = std::make_unique<g::gfx::density_volume<g::gfx::vertex::pos_norm_tan>>(state.world.sdf, generator, offsets);
    terrain->scale = 10; // size of each block in world units
    terrain->depth = 5; // resolution of terrain blocks

}


void game::Renderer::draw(game::State& state, float dt)
{
	auto error = state.player.position - camera.position;
	error[2] = 0;
	camera_velocity += error * 0.01f * dt;

	camera.position += error * 0.1f + camera_velocity * dt;

	camera.aspect_ratio(g::gfx::aspect());
	// camera.look_at(state.player.position);

	// sky
	glDisable(GL_DEPTH_TEST);
	plane.using_shader(assets.shader("sky.vs+sky.fs"))
	.set_camera(camera)
	.draw<GL_TRIANGLE_FAN>();	
	glEnable(GL_DEPTH_TEST);


	glDisable(GL_CULL_FACE);

	terrain->update(camera);
    terrain->draw(camera, assets.shader("terrain.vs+terrain.fs"), [&](g::gfx::shader::usage& usage) {
        // this allows the user to set shader uniforms for each block before it's drawn
        usage["u_model"].mat4(mat4::I());
    });

    for (auto& a : state.abductees)
    {
    	auto& abductee_settings = tweaker->objects[a.obj_name()];
		auto model = mat4::translation(a.position);
		
		plane.using_shader(assets.shader("sprite.vs+sprite.fs"))
		["u_model"].mat4(model)
		["u_tex"].texture(abductee_settings.texture("sprite"))
		.set_camera(camera)
		.draw<GL_TRIANGLE_FAN>();
    }

    // player
	{
		auto model = mat4::translation(state.player.position) * mat4::rotation({0, 0, 1}, state.player.roll);
		auto& player_settings = tweaker->objects["player"];

		plane.using_shader(assets.shader("sprite.vs+sprite.fs"))
		["u_model"].mat4(model)
		["u_tex"].texture(player_settings.texture("sprite"))
		.set_camera(camera)
		.draw<GL_TRIANGLE_FAN>();
	}
}