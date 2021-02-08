#pragma once

#include <entt/entt.hpp>

class Factory {
protected:
	Factory(entt::registry &registry): m_registry(registry) {}
    entt::registry& m_registry;
};
