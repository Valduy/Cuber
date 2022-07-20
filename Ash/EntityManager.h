#pragma once

#include <map>
#include <set>
#include <vector>
#include <functional>
#include "Type.h"
#include "Entity.h"
#include "Filter.h"

namespace ash {

class EntityManager {
public:
	Entity& Create() {
		const auto entity = new Entity();
		entities_.insert(entity);
		entity->ComponentAdded.Attach(*this, &EntityManager::OnComponentAdded);
		entity->ComponentRemoved.Attach(*this, &EntityManager::OnComponentRemoved);
		return *entity;
	}

	void Destroy(Entity& entity) {
		destroy_pool_.push_back(&entity);
		entity.ComponentAdded.Detach(*this, &EntityManager::OnComponentAdded);
		entity.ComponentRemoved.Detach(*this, &EntityManager::OnComponentRemoved);
	}

	void Invalidate() {
		for (const auto it : families_) {
			const auto family = it.second;
			family->Invalidate();

			for (const auto entity : destroy_pool_) {
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
		const Type::Id id = Type::GetId<Family<Args...>>();
		const auto it = families_.find(id);

		if (it != families_.end()) {
			auto* family = static_cast<Family<Args...>*>(it->second);
			return { *family };
		}

		auto family = new Family<Args...>();
		families_[id] = family;

		for (auto entity : entities_) {
			family->TryAddEntity(*entity);
		}

		return { *family };
	}

private:
	std::map<Type::Id, IFamily*> families_;
	std::set<Entity*> entities_;
	std::vector<Entity*> destroy_pool_;

	void OnComponentAdded(Entity& entity, Type::Id id) {
		for (const auto it : families_) {
			const auto family = it.second;
			family->OnComponentAddedToEntity(entity, id);
		}
	}

	void OnComponentRemoved(Entity& entity, Type::Id id) {
		for (const auto it : families_) {
			const auto family = it.second;
			family->OnComponentRemovedFromEntity(entity, id);
		}
	}
};

} // namespace ash