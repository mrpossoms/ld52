#pragma once
#include <g.h>

using namespace xmath;

namespace game
{

struct Player : public g::dyn::particle
{
	float roll = 0;

	vec<3> up()
	{
		return { sinf(roll), cosf(roll), 0 };
	}
};

struct State
{
	Player player;
};

} // namespace game