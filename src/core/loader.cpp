#include "loader.hpp"
#include <nlohmann/json.hpp>
#include "common.hpp"
#include <core/registry.hpp>
#include <core/factories.hpp>


MapState Loader::load_map(Level level)
{
	MapState state{ tile_matrix_dimension };

    nlohmann::json json;
	std::string fileName;
	switch (level)
	{
	case Level::sandbox:
		fileName = "sandbox_map.json";
		break;
	case Level::level1:
		fileName = "level_one_map.json";
		break;
	case Level::level2:
		fileName = "level_two_map.json";
		break;
	case Level::level3:
		fileName = "level_three_map.json";
		break;
	case Level::level4:
		fileName = "level_four_map.json";
		break;
	case Level::level5:
		fileName = "level_five_map.json";
		break;
	case Level::path_finding_debug:
		fileName = "path_finding_debug_map.json";
		break;
    case Level::tutorial:
        fileName = "tutorial_map.json";
        break;
	default:
		fileName = "sandbox_map.json";
	}
	load_json(fileName, json);
	std::string mapString = json["map"];

	auto entities = m_registry.view<Tile>();
	for (int i = 0; i < tile_matrix_dimension.x; i++) {
		for (int j = 0; j < tile_matrix_dimension.y; j++) {
			int index = i + j * tile_matrix_dimension.x;
			if (index >= mapString.length()) {
				state[ivec2(i, j)] = TileType::basic;
			}
			else {
				state[ivec2(i, j)] = char_to_tileType(mapString[index]);
			}
		}
	}
	return state;
}

UnitMapState Loader::load_units(Level level) {
	UnitMapState state{ tile_matrix_dimension };
	for (std::size_t i = 0; i < state.area(); i++) {
		state[i] = UnitType::empty;
	};

	nlohmann::json json;
	std::string fileName;
	switch (level)
	{
	case Level::sandbox:
		fileName = "sandbox_units.json";
		break;
	case Level::level1:
		fileName = "level_one_units.json";
		break;
	case Level::level2:
		fileName = "level_two_units.json";
		break;
	case Level::level3:
		fileName = "level_three_units.json";
		break;
	case Level::level4:
		fileName = "level_four_units.json";
		break;
	case Level::level5:
		fileName = "level_five_units.json";
		break;
	case Level::path_finding_debug:
		fileName = "path_finding_debug_units.json";
		break;
    case Level::tutorial:
        fileName = "tutorial_units.json";
        break;
	default:
		fileName = "sandbox_units.json";
	}
	load_json(fileName, json);
	auto unit_arr = json["unit_arr"];
	if (unit_arr != NULL) {
		for (int i = 0; i < unit_arr.size(); i++) {
			auto unit = unit_arr[i];
			ivec2 tile_index = vec2(unit["position"]["x"], unit["position"]["y"]);
			std::cout << unit["unit_type"] << std::endl;
			state[tile_index] = int_to_unitType(unit["unit_type"]);
		}
	}
	return state;


}
