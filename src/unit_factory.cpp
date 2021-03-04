// Header
#include "unit_factory.hpp"
#include "render.hpp"

UnitFactory::UnitFactory(LevelStateSystem &levelState) : level_state(levelState) {
}

UnitType UnitFactory::curType = H_Monitor;

ECS::Entity UnitFactory::create_unit(vec2 position, UnitType unittype = curType, vec2 coor = {1, 2}) {
    auto entity = ECS::Entity();
    std::string key = "unit";
	switch (unittype) {
		case H_Terminator:
			key = "human_unit";
			break;
		case H_Monitor:
			key = "human_monitor";
			break;
		case H_Archer:
			key = "human_archer";
			break;
		case H_Healer:
			key = "human_healer";
			break;
        case A_Terminator:
            key = "AI_unit";
            break;
        case A_Monitor:
            key = "AI_monitor";
            break;
        case A_Archer:
            key = "AI_archer";
            break;
        case A_Healer:
            key = "AI_healer";
            break;
	}

    ShadedMesh& resource = cache_resource(key);

    if (resource.mesh.vertices.size() == 0)
    {
		switch (unittype) {
			case H_Terminator:
//				resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
//				RenderSystem::createColoredMesh(resource, key);
                resource = ShadedMesh();
                RenderSystem::createSprite(resource, textures_path("close-001.png"), "textured");

                break;
			case H_Monitor:
				resource = ShadedMesh();
				RenderSystem::createSprite(resource, textures_path("tank-001.png"), "textured");
				break;
			case H_Archer:
				resource = ShadedMesh();
				RenderSystem::createSprite(resource, textures_path("long-d-001.png"), "textured");
				break;
			case H_Healer:
				resource = ShadedMesh();
				RenderSystem::createSprite(resource, textures_path("recover-001.png"), "textured");
				break;
            case A_Terminator:
//				resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
//				RenderSystem::createColoredMesh(resource, key);
                resource = ShadedMesh();
                RenderSystem::createSprite(resource, textures_path("ai_short-d.png"), "textured");

                break;
            case A_Monitor:
                resource = ShadedMesh();
                RenderSystem::createSprite(resource, textures_path("ai_tank-2.png"), "textured");
                break;
            case A_Archer:
                resource = ShadedMesh();
                RenderSystem::createSprite(resource, textures_path("ai_long-d.png"), "textured");
                break;
            case A_Healer:
                resource = ShadedMesh();
                RenderSystem::createSprite(resource, textures_path("ai_recover.png"), "textured");
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
		case H_Terminator:
//			resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
//			RenderSystem::createColoredMesh(resource, key);
            motion.scale = vec2(gridWidth*2.5, gridHeight*2.5);
            property.hp = 1000;
			break;
		case H_Monitor:
			motion.scale = vec2(gridWidth, gridHeight);
			property.hp = 1000;
			break;
		case H_Archer:
			motion.scale = vec2(gridWidth*3, gridHeight*3);
			property.attackRangeScale = 3;
			break;
		case H_Healer:
			motion.scale = vec2(gridWidth*2.5, gridHeight*2.5);
			property.attackRangeScale = 3;
			break;
        case A_Terminator:
//			resource.mesh.loadFromOBJFile(mesh_path("unit1.obj"));
//			RenderSystem::createColoredMesh(resource, key);
            motion.scale = vec2(gridWidth*1.5, gridHeight*1.5);
            property.hp = 1000;
            break;
        case A_Monitor:
            motion.scale = vec2(gridWidth, gridHeight);
            property.hp = 1000;
            break;
        case A_Archer:
            motion.scale = vec2(gridWidth, gridHeight);
            property.attackRangeScale = 3;
            break;
        case A_Healer:
            motion.scale = vec2(gridWidth, gridHeight);
            property.attackRangeScale = 3;
            break;
	}

	property.unitType = unittype;
    return entity;

}
void UnitFactory::on_key_click(int key, int action) {
	if (action == GLFW_PRESS && key == GLFW_KEY_W)
	{
		should_place_enemy = SHOULD_PLACE_HUMAN;
		curType = H_Terminator;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_A)
	{
		should_place_enemy = SHOULD_PLACE_HUMAN;
		curType = H_Monitor;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_S)
	{
		should_place_enemy = SHOULD_PLACE_HUMAN;
		curType = H_Archer;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_D)
	{
		should_place_enemy = SHOULD_PLACE_HUMAN;
		curType = H_Healer;
	}



	if (action == GLFW_PRESS && key == GLFW_KEY_UP)
	{
		should_place_enemy = SHOULD_PLACE_ENEMY;
		curType = A_Terminator;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
	{
		should_place_enemy = SHOULD_PLACE_ENEMY;
		curType = A_Monitor;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
	{
		should_place_enemy = SHOULD_PLACE_ENEMY;
		curType = A_Archer;
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT)
	{
		should_place_enemy = SHOULD_PLACE_ENEMY;
		curType = A_Healer;
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
