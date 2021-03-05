// Header
#include "unit_factory.hpp"
#include "render.hpp"
#include <iostream>

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
    if(unit_type==MONITOR){
        motion.scale = vec2(gridWidth,gridHeight);
        property.hp = 1000;
    }

    Transform transform;
    transform.translate(motion.position);
    transform.rotate(motion.angle);
    transform.scale(motion.scale);

    if (key == "monitor" ) {
        BoundingBox& boundingBox = ECS::registry<BoundingBox>.emplace(entity);
        boundingBox.vertices = { vec2(0.f, resource.mesh.original_size.y / 4), vec2(resource.mesh.original_size.x / 2, 0.f), vec2(resource.mesh.original_size.x, resource.mesh.original_size.y / 4),
                                vec2(resource.mesh.original_size.x, 3* resource.mesh.original_size.y / 4), vec2(resource.mesh.original_size.x / 2, resource.mesh.original_size.y), vec2(0.f, 3 * resource.mesh.original_size.y / 4) };
        for (int i = 0; i < boundingBox.vertices.size(); i++) {
            boundingBox.vertices[i] = transform.mat * vec3(boundingBox.vertices[i].x, boundingBox.vertices[i].y, 1.f);

            //std::cout << boundingBox.vertices[i].x << "," << boundingBox.vertices[i].y << std::endl;
        }
        
    }
    else if (key == "unit") {
        BoundingBox& boundingBox = ECS::registry<BoundingBox>.emplace(entity);
        auto& texmesh = *ECS::registry<ShadedMeshRef>.get(entity).reference_to_cache;
        float top = 0;
        float bottom = 0;
        float right = 0;
        float left = 0;
        for (ColoredVertex pos : texmesh.mesh.vertices) {
            auto position = pos.position;
            //std::cout << position.x << "," << position.y << std::endl;
            position = transform.mat * vec3(position.x, position.y, 1.f);
            if (position.x >= right) {
                right = position.x;
            }
            if (position.x <= left) {
                left = position.x;
            }
            if (position.y >= bottom) {
                bottom = position.y;
            }
            if (position.y <= top) {
                top = position.y;
            }
        }
        //std::cout << top << "," << right << "," << bottom << "," << left << std::endl;
        boundingBox.vertices = { vec2(left, top), vec2(right, top), vec2(right, bottom), vec2(left, bottom) };
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
