#pragma once

#include "common.hpp"

// Data structure for pebble-specific information
namespace DebugSystem {
	extern bool in_debug_mode;

	// draw a red line for debugging purposes
	void createLine(vec2 position, vec2 size);


    void createDirectTri(vec2 position, vec2 scale);
	// !!! TODO A2: implement debugging of bounding boxes
	void createBox(ECS::Entity entity, vec2 scale);

	// Removes all debugging graphics in ECS, called at every iteration of the game loop
	void clearDebugComponents();
};
