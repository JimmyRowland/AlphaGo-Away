#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Unit
{
	// Creates all the associated render resources and default transform
	static ECS::Entity createUnit(vec2 pos);

};
