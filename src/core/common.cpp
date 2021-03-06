#include "common.hpp"

// Note, we could also use the functions from GLM but we write the transformations here to show the uderlying math
void Transform::scale(vec2 scale)
{
	mat3 S = { { scale.x, 0.f, 0.f },{ 0.f, scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * S;
}

void Transform::rotate(float radians)
{
	float c = std::cos(radians);
	float s = std::sin(radians);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };
	mat = mat * R;
}

void Transform::translate(vec2 offset)
{
	mat3 T = { { 1.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ offset.x, offset.y, 1.f } };
	mat = mat * T;
}

void load_json(const std::string& file_name, nlohmann::json& json){
    std::ifstream i(json_path(file_name));
    i >> json;
    std::cout << "Loaded " << json_path(file_name) << "\n";
};
void save_json(const std::string& file_name, nlohmann::json& json){
    std::ofstream o(json_path(file_name));
    o << std::setw(4) << json << std::endl;
    std::cout << "Saved " << json_path(file_name) << "\n";
}