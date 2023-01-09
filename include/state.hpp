#pragma once
#include <g.h>

#include "tweaker.hpp"

using namespace xmath;

namespace game
{

struct Player : public g::dyn::particle, g::dyn::cd::ray_collider
{
	float roll = 0;
	float hoovering = 0;

    std::vector<ray>& rays() override
    {
    	ray_list.clear();
    	ray_list.push_back({position - vec<3>{0, 0.5, 0}, velocity});
    	return ray_list;
    }


	vec<3> up()
	{
		return { sinf(roll), cosf(roll), 0 };
	}
};

struct Abductee : public g::dyn::particle, g::dyn::cd::ray_collider
{
	enum Type
	{
		dog = 0,
		cow,
		chicken,
		farmer,
	};

	unsigned type;

    std::vector<ray>& rays() override
    {
    	ray_list.clear();
    	ray_list.push_back({position - vec<3>{0, 0.5, 0}, velocity});
    	return ray_list;
    }

    const std::string& obj_name() const
    {
    	const static std::string names[] = {
    		"abductee.dog",
    		"abductee.cow",
    		"abductee.chicken",
    		"abductee.farmer",
    	};

    	return names[(unsigned)type];
    }
};

struct World
{
	struct Tile
	{
		vec<3> corners[2];

	};

	g::game::sdf sdf;
};

struct State
{
	std::shared_ptr<game::Tweaker> tweaker;
	
	Player player;
	std::vector<Abductee> abductees;
	World world;
	float time;
};

} // namespace game