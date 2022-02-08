#pragma once

#include <vector>
#include <unordered_set>
#include "Entity.h"
#include "FilterFactory.h"

namespace ecs {

class EntityManager {
public:
	using Action = std::function<void(Entity&)>;

	template<typename First, typename... Rest>
	void For(Action action) const {
		const Signature sample = Signer::GetSignature<First, Rest...>();

		for (const auto it : entities_) {
			if (Signer::IsMatch(it->GetSignature(), sample))
			action(*it);
		}
	}

	Entity& AddEntity() {
		const auto entity = new Entity();
		entities_.insert(entity);
		return *entity;
	}
	
	bool RemoveEntity(Entity& entity) {
		const auto it = entities_.find(&entity);

		if (it != entities_.end()) {
			delete *it;
			entities_.erase(it);
		}

		return false;
	}

private:
	std::unordered_set<Entity*> entities_;
};

} // namespace ecs

