#pragma once
#include "Utils.h"

struct Light
{
	Vec3f color;
	Vec2f center;
	float vanish;
	float distance;
	bool active = true;
};

