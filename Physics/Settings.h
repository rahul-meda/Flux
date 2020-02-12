#pragma once

#define PI 3.14159265359f
#define fxEpsilon FLT_EPSILON

// max no.of contact points b/w 2 shapes
#define maxManifoldPoints 4

// collision and constraint tolerance
#define linearSlop 0.005f

#define linearSlop2 (linearSlop * linearSlop)

#define hullRadius (2.0f * linearSlop)

#define hullRadiusSum (2.0f * hullRadius)

// velocity slop allowance for elastic collisions
#define velocityThreshold 1.0f

#define maxTranslation 1.2f
#define maxTranslation2 (maxTranslation * maxTranslation)

#define maxRotation (0.3f * PI)
#define maxRotation2 (maxRotation * maxRotation)

#define baumgarte 0.2f