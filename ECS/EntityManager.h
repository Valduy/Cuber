#pragma once

#include <functional>
#include <unordered_set>
#include "Entity.h"

namespace ecs {

class EntityManager {
public:
	using Action = std::function<void(Entity&)>;

	~EntityManager() {
		for (const auto it : entities_) {
			delete it;
		}

		entities_.clear();
	}

	template<typename First, typename... Rest>
	void For(Action action) const {
		const Signer::Signature sample = Signer::GetSignature<First, Rest...>();

		for (const auto it : entities_) {
			if (Signer::IsMatch(it->GetSignature(), sample)) {
				action(*it);
			}			
		}
	}

	Entity& CreateEntity() {
		auto it = entities_.insert(new Entity());
		return *(*it.first);
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

