#pragma once

#include "core/common.hpp"
#include <entt/entt.hpp>
#include "render_components.hpp"
#include "core/registry.hpp"
#include "components/motion.hpp"
#include "components/tile.hpp"
#include "components/units.hpp"
struct InstancedMesh;
struct ShadedMesh;

// OpenGL utilities
void gl_has_errors();
// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem
{
public:
	// Initialize the window
    RenderSystem(GLFWwindow &window);

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
    void draw(vec2 window_size_in_game_units);

	// Expose the creating of visual representations to other systems
	static void createSprite(ShadedMesh& mesh_container, std::string texture_path, std::string shader_name);
	static void createColoredMesh(ShadedMesh& mesh_container, std::string shader_name);

private:
	// Initialize the screeen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water shader
    void initScreenTexture();

	// Internal drawing functions for each entity type
    void drawTexturedMesh(entt::entity, const mat3 &projection);
	void drawToScreen();

	// Window handle
	GLFWwindow& window;

	// Screen texture handles
	GLuint frame_buffer;
	ShadedMesh screen_sprite;
	GLResource<RENDER_BUFFER> depth_render_buffer_id;
    entt::entity screen_state_entity;
    float frame_num = 0.f;
};
