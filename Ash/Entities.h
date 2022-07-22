#pragma once

#include "EntityManager.h"

namespace ash {

class Entities {
public:
	Entities(EntityManager& entity_manager)
		: entity_manager_(entity_manager)
	{}

	Entity& Create() const {
		return entity_manager_.Create();
	}

	void Destroy(Entity& entity) const {
		entity_manager_.Destroy(entity);
	}

	template<typename... Args>
	Filter<Args...> Filter() {
		return entity_manager_.Filter<Args...>();
	}

private:
	EntityManager& entity_manager_;
};

} // namespace ash