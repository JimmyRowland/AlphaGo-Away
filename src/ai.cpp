// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "a_star.hpp"
#include <iostream>

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE FISH AI HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will likely want to write new functions and need to create
	// new data structures to implement a more sophisticated Fish AI. 
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	// Path finding can be done as follows, the pairs are the grid positions of the current units 

	/*A_Star a = A_Star();
	auto path = a.getPath(std::make_pair(9, 1), std::make_pair(4, 2));
	// printing for debugging reasons, feel free to use, or just delete if you don't
	for (int i = 0; i < path.size(); ++i) {
		std::cout << path[i].first << ", " << path[i].second << std::endl;
	}*/

}
