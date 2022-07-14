#pragma once

#include <cassert>
#include <functional>
#include <map>
#include "Type.h"

namespace ash {

class Entity {
public:
	class ComponentBase {
		
		friend class Entity;

	public:
		Entity& GetOwner() const {
			assert(owner_ != nullptr && "Component isn't initialized.");
			return *owner_;
		}

		ComponentBase()
			: owner_(nullptr)
		{}

	protected:	
		virtual void OnAdd(Entity& owner) {
			owner_ = &owner;
		}

		virtual void OnRemove() {}

	private:
		Entity* owner_;
	};

	template<typename TComponent>
	using Factory = std::function<TComponent* ()>;

	Entity() = default;
	Entity(const Entity&) = delete;

	~Entity() {
		for (const auto& it : components_map_) {
			delete it.second;
		}

		components_map_.clear();
	}

	template<typename TComponent>
	TComponent& Add() {
		return Add<TComponent>([]() {
			return new TComponent();
		});
	}

	template<typename TComponent>
	TComponent& Add(Factory<TComponent> factory) {
		const auto component_id = Type::GetId<TComponent>();
		const auto it = components_map_.find(component_id);
		TComponent* component = factory();
		
		if (it != components_map_.end()) {
			it->second->OnRemove();
			delete it->second;
			it->second = component;
			component->OnAdd(*this);
		}
		else {
			components_map_[component_id] = component;
			component->OnAdd(*this);
		}

		return *component;
	}

	template<typename TComponent>
	bool Remove() {
		const auto component_id = Type::GetId<TComponent>();
		const auto it = components_map_.find(component_id);

		if (it != components_map_.end()) {
			it->second->OnRemove();
			delete it->second;
			components_map_.erase(it);
			return true;
		}

		return false;
	}

	template<typename TComponent>
	bool Contain() {
		return Contain(Type::GetId<TComponent>());
	}

	bool Contain(Type::Id id) {
		const auto it = components_map_.find(id);
		return it != components_map_.end();
	}

	template<typename TComponent>
	TComponent& Get() {
		auto component = Get(Type::GetId<TComponent>());
		return static_cast<TComponent&>(component);
	}

	ComponentBase& Get(Type::Id id) {
		const auto it = components_map_.find(id);
		assert(it != components_map_.end() && "Entity don't contain component.");
		return *it->second;
	}

private:
	std::map<Type::Id, ComponentBase*> components_map_;
};

} // namespace ash