#pragma once

#include <cassert>
#include <functional>
#include <map>
#include "../Utils/Delegate.h"
#include "../Utils/Event.h"
#include "Type.h"

namespace ash {

class Entity {
private:
	utils::Delegate<Entity&, Type::Id> component_added_;
	utils::Delegate<Entity&, Type::Id> component_removed_;

public:
	const utils::Event<Entity&, Type::Id> ComponentAdded = {component_added_};
	const utils::Event<Entity&, Type::Id> ComponentRemoved = {component_removed_};

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

		virtual ~ComponentBase() = default;

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

		component_added_.Notify(*this, component_id);
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
			component_removed_.Notify(*this, component_id);
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
		Type::Id id = Type::GetId<TComponent>();
		return static_cast<TComponent&>(Get(id));
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