#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Army
{
	// Creates all the associated render resources and default transform
	static ECS::Entity createArmy(vec2 pos);
};


struct SwordMan : Army 
{
	static ECS::Entity createSwordMan(vec2 pos);
};