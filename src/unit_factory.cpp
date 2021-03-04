// Header
#include "unit_factory.hpp"
#include "render.hpp"

UnitFactory::UnitFactory(LevelStateSystem &levelState) : level_state(levelState) {
}

UnitType UnitFactory::curType = Monitor;

ECS::Entity UnitFactory::create_unit(vec2 position, UnitType unittype = curType, vec2 coor = {1, 2}) {
    auto entity = ECS::Entity();
    std::string key = "unit";
	switch (unittype) {
		case Terminator:
			key = "unit";
			break;
		case Monitor:
			key = "monitor";
			break;
		case Archer:
			key = "archer";
			break;
		case Healer:
			key = "healer";
			break;
	}

    ShadedMesh& resource = cache_resource(key);

    if (resource.mesh.vertices.size() == 0)
    {
		switch (unittype) {
			case Terminator:
				resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
				RenderSystem::createColoredMesh(resource, key);
				break;
			case Monitor:
				resource = ShadedMesh();
				RenderSystem::createSprite(resource, textures_path("ai_default.png"), "textured");
				break;
			case Archer:
				resource = ShadedMesh();
				RenderSystem::createSprite(resource, textures_path("default_unit.png"), "textured");
				break;
			case Healer:
				resource = ShadedMesh();
				RenderSystem::createSprite(resource, textures_path("ai_default_2.png"), "textured");
				break;
		}
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Setting initial motion values
    Motion& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = position;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
	motion.coor = coor;
    motion.scale = resource.mesh.original_size * 150.f;
    ECS::registry<Unit>.emplace(entity);
    Property& property = ECS::registry<Property>.emplace(entity);

    if(should_place_enemy){
        property.isEnemy=1;
    }else{
        property.isEnemy=0;
        level_state.update_health();
    }

	switch (unittype) {
		case Terminator:
			resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
			RenderSystem::createColoredMesh(resource, key);
			break;
		case Monitor:
			motion.scale = vec2(gridWidth, gridHeight);
			property.hp = 1000;
			break;
		case Archer:
			motion.scale = vec2(gridWidth, gridHeight);
			property.attackRangeScale = 3;
			break;
		case Healer:
			motion.scale = vec2(gridWidth, gridHeight);
			property.attackRangeScale = 3;
			break;
	}

	property.unitType = unittype;
    return entity;

}
void UnitFactory::on_key_click(int key, int action) {
	if (action == GLFW_PRESS && key == GLFW_KEY_Q) 
	{
		should_place_enemy = SHOULD_PLACE_HUMAN;
		curType = Terminator;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_W)
	{
		should_place_enemy = SHOULD_PLACE_HUMAN;
		curType = Monitor;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_E)
	{
		should_place_enemy = SHOULD_PLACE_HUMAN;
		curType = Archer;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_R)
	{
		should_place_enemy = SHOULD_PLACE_HUMAN;
		curType = Healer;
	}



	if (action == GLFW_PRESS && key == GLFW_KEY_A)
	{
		should_place_enemy = SHOULD_PLACE_ENEMY;
		curType = Terminator;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_S)
	{
		should_place_enemy = SHOULD_PLACE_ENEMY;
		curType = Monitor;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_D)
	{
		should_place_enemy = SHOULD_PLACE_ENEMY;
		curType = Archer;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_F)
	{
		should_place_enemy = SHOULD_PLACE_ENEMY;
		curType = Healer;
	}
}

void UnitFactory::setGridWidth(float gridWidth) {
    UnitFactory::gridWidth = gridWidth;
}

void UnitFactory::setGridHeight(float gridHeight) {
    UnitFactory::gridHeight = gridHeight;
}

LevelStateSystem& UnitFactory::getLevelState() const {
    return level_state;
}
