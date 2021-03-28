#ifndef LEVEL_LOADER_HPP
#define LEVEL_LOADER_HPP

#include <string>
#include <core/map.hpp>
#include <core/unitmap.hpp>


class Loader {

public:


	MapState load_map(Level level);

	UnitMapState load_units(Level level);

	void save_map(Level level);
};

#endif