#pragma once
#include <g.h>

#include "tweaker.hpp"

using namespace xmath;

namespace game
{

struct Player : public g::dyn::particle, g::dyn::cd::ray_collider
{
	float roll = 0;

    std::vector<ray>& rays() override
    {
    	ray_list.clear();
    	ray_list.push_back({position, velocity});
    	return ray_list;
    }


	vec<3> up()
	{
		return { sinf(roll), cosf(roll), 0 };
	}
};

struct World
{

};

struct State
{
	std::shared_ptr<game::Tweaker> tweaker;
	
	Player player;
	World world;
};

} // namespace game