// internal
#include "ai.hpp"

void ai_update(float elapsed_ms)
{
    if (((int) (floor(elapsed_ms))) % 5 != 0) {
        return;
    }
    A_Star a = A_Star(std::make_pair(tile_matrix_dimension.x, tile_matrix_dimension.y));
    for (auto &&[entity, property, position]: m_registry.view<UnitProperty, Position>().each())
    {
        if (!m_registry.valid(property.actualTarget) || property.actualTarget == entity) {
            property.path = {};
        }else{
            if(m_registry.valid(property.actualTarget)){
                auto&& [target_position, target_property] = m_registry.get<Position, UnitProperty>(property.actualTarget);
                auto target_tile_index = get_tile_index(target_position.position);
                auto entity_tile_index = get_tile_index(position.position);
                property.path= a.getPath(std::make_pair(entity_tile_index.x, entity_tile_index.y), std::make_pair(target_tile_index.x, target_tile_index.y));
            }
        }
    }
}