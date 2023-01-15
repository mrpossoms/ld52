#pragma once
#include <g.h>

#include "tweaker.hpp"

using namespace xmath;

namespace game
{

static float random_norm()
{
	return ((::rand() % 2048) - 1024) / 1024.f;
}


struct Player : public g::dyn::particle, g::dyn::cd::ray_collider
{
	float roll = 0;
	float hoovering = 0;

    std::vector<ray>& rays() override
    {
    	ray_list.clear();
    	ray_list.push_back({position - vec<3>{0, 0.25, 0}, velocity});
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
	g::gfx::sprite::instance sprite;

	struct {
		float speed;
		float count_down;

		void next()
		{
			count_down = (random_norm() + 1) * 10;
			speed = random_norm() * 10.f;
		}
	} move;

    std::vector<ray>& rays() override
    {
    	const vec<3> offsets[] = {
    		{0, 0.125, 0},
    		{0, 0.25, 0},
    		{0, 0.0625, 0},
    		{0, 0.25, 0}
    	};

    	ray_list.clear();
    	ray_list.push_back({position - offsets[type], velocity});
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

struct Prop
{
	unsigned type;
	vec<3> position;

    const std::string& obj_name() const
    {
    	const static std::string names[] = {
    		"Tree.json",
    		"Rock.json",
    		"Haybale.json",
    		"Haybale.json",
    	};

    	return names[(unsigned)type];
    }
};

struct World
{
	std::unordered_set<int> spawns;

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