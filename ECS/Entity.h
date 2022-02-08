#pragma once

#include <cassert>
#include <map>
#include "BitIdentifier.h"
#include "IComponent.h"
#include "Signer.h"

namespace ecs {

class Entity {
public:
	Entity()
		: signature_(0)
	{}

	~Entity() {
		for (const auto it : components_map_) {
			delete it.second;
		}

		components_map_.clear();
	}

	Signer::Signature GetSignature() const {
		return signature_;
	}

	template<typename TComponent>
	TComponent& Add() {
		const ComponentId component_id = BitIdentifier::GetId<TComponent>();
		const auto it = components_map_.find(component_id);
		TComponent* component = new TComponent();

		if (it != components_map_.end()) {
			delete it->second;			
			it->second = component;
		}
		else {
			components_map_[component_id] = component;
			signature_ |= component_id;
		}

		return component;
	}

	template<typename TComponent>
	bool Remove() {
		const ComponentId component_id = BitIdentifier::GetId<TComponent>();
		const auto it = components_map_.find(component_id);

		if (it != components_map_.end()) {
			components_map_.erase(it);
			signature_ &= ~component_id;
			return true;
		}

		return false;
	}

	template<typename TComponent>
	bool Contain() {
		const ComponentId component_id = BitIdentifier::GetId<TComponent>();
		const auto it = components_map_.find(component_id);
		return it != components_map_.end();
	}

	template<typename TComponent>
	TComponent& Get() {
		const ComponentId component_id = BitIdentifier::GetId<TComponent>();
		const auto it = components_map_.find(component_id);
		assert(it != components_map_.end() && "Entity don't contain component.");
		return *static_cast<TComponent*>(it->second);
	}

private:
	Signer::Signature signature_;
	std::map<BitIdentifier::Id, IComponent*> components_map_;
};

} // namespace ecs