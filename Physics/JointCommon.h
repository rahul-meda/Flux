
#pragma once

#include <vector>
#include "Settings.h"

struct Position;
struct Velocity;

struct SolverDef
{
	std::vector<Position>* positions;
	std::vector<Velocity>* velocities;
};