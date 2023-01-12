#pragma once

#include "state.hpp"
#include "tweaker.hpp"

#include <g.h>

namespace game
{
namespace gameplay
{

void update(game::State& state, float dt);

float surface_at_x(const game::State& state, float x);

}
}