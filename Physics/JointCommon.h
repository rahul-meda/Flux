
#pragma once

#include <vector>

struct Position;
struct Velocity;

struct SolverDef
{
	std::vector<Position>* positions;
	std::vector<Velocity>* velocities;
};