#pragma once

#include "state.hpp"
#include "tweaker.hpp"

#include <g.h>

namespace game
{
namespace gameplay
{

void update_player(game::State& state, float dt);

void update_dynamics(game::State& state, float dt);

void update(game::State& state, float dt);

}
}