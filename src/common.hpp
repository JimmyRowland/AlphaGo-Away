#pragma once

// stlib
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3

#include "tiny_ecs.hpp"

using namespace glm;
static const float PI = 3.14159265359f;

// Simple utility functions to avoid mistyping directory name
inline std::string data_path() { return "data"; };
inline std::string shader_path(const std::string& name) { return data_path() + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + name; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + name; };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + name; };

const int ALLY = 0;
const int ENEMY = 1;

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
struct Transform {
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

enum UnitType {
	H_Terminator,
	H_Monitor,
	H_Archer,
	H_Healer,
    A_Terminator,
    A_Monitor,
    A_Archer,
    A_Healer
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	// if trigger the battle, the entity start to move and trigger = 1
	int trigger = 0;
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	int state = 0; // 0: stands still; 1: moving; 2: dying
	vec2 coor = { 0,0 };
	std::pair<int, int> gridPos = std::make_pair(0, 0);
};

struct Property {
	int hp = 100;
	int maxhp = 100;
	int attackRange = 5;
	int moveRange = 5;
	int damage = 5;
	int isEnemy = ALLY;
	ECS::Entity target;
	bool selected = false;

	int attackRangeScale = 1; // This is only for M2, will use attack Range for later 
	UnitType unitType;
};

struct GridProperty {
	int type = 0;
};

float get_velocity_after_drag(float velocity_x);

struct BoundingBox {
	std::vector<vec2> vertices;
	std::vector<vec2> transformed_vertices;
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

