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


struct Abductee : public g::dyn::particle, g::dyn::cd::ray_collider
{
	enum Type
	{
		dog = 0,
		cow,
		chicken,
		chicken2,
		farmer,
		COUNT,
		fuel,
	};

	unsigned type;
	g::gfx::sprite::instance sprite;
	float angle = 0;
	float ang_vel = 0;

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
    		{0, 0.0625, 0},
    		{0, 0.25, 0},
    		{},
    		{0, 0.125, 0},
    	};

    	ray_list.clear();
    	ray_list.push_back({position - offsets[type], velocity});
    	return ray_list;
    }

    static const std::string& obj_name_for_type(unsigned t)
    {
    	const static std::string names[] = {
    		"abductee.dog",
    		"abductee.cow",
    		"abductee.chicken",
			"abductee.chicken2",
    		"abductee.farmer",
    		"",
    		"abductee.fuel"
    	};

    	return names[(unsigned)t];
    }

    const std::string& obj_name() const
    {
    	return Abductee::obj_name_for_type(type);
    }
};

struct Player : public g::dyn::particle, g::dyn::cd::ray_collider
{
	float roll = 0;
	float hoovering = 0;
	float energy = 100;
	float thrust = 0;

	bool thrusted = false;
	bool rolled = false;
	bool hoovered = false;

	unsigned abductee_counts[Abductee::Type::COUNT] = {};
	unsigned abductee_targets[Abductee::Type::COUNT] = {};

	g::gfx::sprite::instance sprite;

	float weight()
	{
		float w = 0;
		for (unsigned i = 0; i < Abductee::Type::COUNT; i++)
		{
			w += abductee_counts[i] * 0.125f;
		}

		return w;
	}

	void reset()
	{
		energy = 100;
		memset(abductee_counts, 0, sizeof(abductee_counts));
		velocity *= 0;

		for (unsigned i = 0; i < Abductee::Type::COUNT; i++)
		{
			abductee_targets[i] = ::rand() % 10;
		}

		sprite.track("default");
	}

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