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
	default:
		fileName = "sandbox_map.json";
	}
	load_json(fileName, json);
	auto map_arr = json["map"];
	//std::string unit_arr = json["map"];

	/*
	auto entities = m_registry.view<Tile>();
	for (int i = 0; i < tile_matrix_dimension.x; i++) {
		for (int j = 0; j < tile_matrix_dimension.y; j++) {
			int index = i + j * tile_matrix_dimension.x;
			if (index >= unit_arr.length()) {
				state[ivec2(i, j)] = TileType::basic;
			}
			else {
				state[ivec2(i, j)] = char_to_tileType(unit_arr[index]);
			}
		}
	}*/

	if (map_arr != NULL) {
		for (int i = 0; i < map_arr.size(); i++) {
			auto tile = map_arr[i];
			ivec2 tile_index = get_tile_index(vec2(tile["position"]["x"], tile["position"]["y"]));
			//std::cout << tile["tile_type"] << std::endl;
			state[tile_index] = int_to_tileType(tile["tile_type"]);
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
	default:
		fileName = "sandbox_units.json";
	}
	load_json(fileName, json);
	auto unit_arr = json["unit_arr"];
	if (unit_arr != NULL) {
		for (int i = 0; i < unit_arr.size(); i++) {
			auto unit = unit_arr[i];
			ivec2 tile_index = vec2(unit["position"]["x"], unit["position"]["y"]);
			//std::cout << unit["unit_type"] << std::endl;
			state[tile_index] = int_to_unitType(unit["unit_type"]);
		}
	}
	return state;
}

void Loader::save_map(Level level) {
	nlohmann::json json;
	json["map"] = nlohmann::json::array();

	for (auto& entity : m_registry.view<Tile>()) {
		auto& position = m_registry.get<Position>(entity);
		auto& tile = m_registry.get<Tile>(entity);
		nlohmann::json tile_obj = {
			{"position", {
				{"x", position.position.x},
				{"y", position.position.y}
			}},
			{"tile_type", tileType_to_int(tile.type)}
		};
		json["map"].push_back(tile_obj);
		//std::cout << json["map"] << std::endl;
	}
	//std::cout << json << std::endl;
	//std::ofstream o(json_path("sandbox_save_map.json"));
	//o << std::setw(4) << json << std::endl;
	save_json("sandbox_map.json", json);
	save_json("level_one_map.json", json);
	save_json("level_two_map.json", json);
	save_json("level_three_map.json", json);
	save_json("level_four_map.json", json);
	save_json("level_five_map.json", json);
	save_json("path_finding_debug_map.json", json);
}