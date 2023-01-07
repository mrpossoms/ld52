#pragma once
#include <g.h>
#include <unordered_map>

namespace game
{

struct Tweaker
{

	std::unordered_map<std::string, g::game::object> objects;

	Tweaker(g::asset::store& assets);
};

}