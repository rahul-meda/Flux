#pragma once

#define PI 3.14159265359f
#define fEpsilon FLT_EPSILON

// max no.of contact points b/w 2 shapes
#define maxManifoldPoints 4

// collision and constraint tolerance
#define linearSlop 0.005f

#define hullRadius (2.0f * linearSlop)

// velocity slop allowance for elastic collisions
#define velocityThreshold 1.0f

#define baumgarte 0.2f