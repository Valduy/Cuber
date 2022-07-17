#pragma once

#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <functional>
#include "Type.h"
#include "Entity.h"
#include "Filter.h"
#include "Signature.h"

namespace ash {

class EntityManager {
public:
	Entity& Create() {
		Entity* entity = new Entity();
		entities_.insert(entity);
		return *entity;
	}

	void Destroy(Entity& entity) {
		destroy_pool_.push_back(&entity);
	}

	void Invalidate() {
		for (auto it : families_) {
			auto family = it.second;
			family->Invalidate();

			for (auto entity : destroy_pool_) {
				family->RemoveEntity(*entity);
			}
		}

		for (auto entity : destroy_pool_) {
			entities_.erase(entity);
		}

		destroy_pool_.clear();
	}

	template<typename... Args>
	Filter<Args...> Filter() {
		Type::Id id = Type::GetId<Family<Args...>>();
		auto it = families_.find(id);

		if (it != families_.end()) {
			return { static_cast<Family<Args...>*>(*it) };
		}

		auto family = new Family<Args...>();
		families_[id] = family;

		for (auto entity : entities_) {
			family.TryAddEntity(*entity);
		}

		return { static_cast<Family<Args...>*>(*it) };
	}

private:
	std::map<Type::Id, IFamily*> families_;
	std::set<Entity*> entities_;
	std::vector<Entity*> destroy_pool_;

};

} // namespace ash