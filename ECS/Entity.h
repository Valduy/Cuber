#pragma once

#include <cassert>
#include <map>
#include "BitIdentifier.h"
#include "IComponent.h"
#include "Signer.h"

namespace ecs {

class Entity {
public:
	template<typename TComponent>
	using Factory = std::function<TComponent* ()>;

	Entity()
		: signature_(0)
	{}

	~Entity() {
		for (const auto& it : components_map_) {
			delete it.second;
		}

		components_map_.clear();
	}

	Signer::Signature GetSignature() const {
		return signature_;
	}

	template<typename TComponent>
	TComponent& Add() {
		return Add<TComponent>([]() {
			return new TComponent();
		});
	}

	template<typename TComponent>
	TComponent& Add(Factory<TComponent> factory) {
		const auto component_id = BitIdentifier::GetId<TComponent>();
		const auto it = components_map_.find(component_id);
		TComponent* component = factory();

		if (it != components_map_.end()) {
			delete it->second;			
			it->second = component;
		}
		else {
			components_map_[component_id] = component;
			signature_ |= component_id;
		}

		return *component;
	}

	template<typename TComponent>
	bool Remove() {
		const auto component_id = BitIdentifier::GetId<TComponent>();
		const auto it = components_map_.find(component_id);

		if (it != components_map_.end()) {
			delete it->second;
			components_map_.erase(it);			
			signature_ &= ~component_id;
			return true;
		}

		return false;
	}

	template<typename TComponent>
	bool Contain() {
		const auto component_id = BitIdentifier::GetId<TComponent>();
		const auto it = components_map_.find(component_id);
		return it != components_map_.end();
	}

	template<typename TComponent>
	TComponent& Get() {
		const auto component_id = BitIdentifier::GetId<TComponent>();
		const auto it = components_map_.find(component_id);
		assert(it != components_map_.end() && "Entity don't contain component.");
		return *static_cast<TComponent*>(it->second);
	}

private:
	Signer::Signature signature_;
	std::map<BitIdentifier::Id, IComponent*> components_map_;
};

} // namespace ecs
