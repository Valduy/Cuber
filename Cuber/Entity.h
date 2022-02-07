#pragma once

#include <cstdint>
#include <map>
#include "ComponentsIdentifier.h"
#include "Signer.h"

namespace ecs {

class Entity {
public:
	Entity()
		: signature_(0)
	{}

	~Entity() {
		for (auto it : components_map_) {
			delete it.second;
		}

		components_map_.clear();
	}

	Signature GetSignature() const {
		return signature_;
	}

	template<typename TSource>
	TSource* const Add() {
		return Add<TSource>([]() { return new TSource(); });
	}

	template<typename TSource, typename Factory>
	TSource* const Add(Factory factory) {
		ComponentId component_id = ComponentsIdentifier::GetComponentId<TSource>();

		if (component_id == 0) {
			return nullptr;
		}

		auto it = components_map_.find(component_id);

		if (it != components_map_.end()) {
			return (TSource*)it->second;
		}

		TSource* component = factory();
		components_map_[component_id] = component;
		signature_ |= component_id;
		return component;
	}

	template<typename TSource>
	bool Remove() {
		ComponentId component_id = ComponentsIdentifier::GetComponentId<TSource>();
		auto it = components_map_.find(component_id);

		if (it != components_map_.end()) {
			components_map_.erase(it->first);
			signature_ &= ~component_id;
			return true;
		}

		return false;
	}

	template<typename TSource>
	TSource* const Get() {
		ComponentId component_id = ComponentsIdentifier::GetComponentId<TSource>();
		auto it = components_map_.find(component_id);

		if (it != components_map_.end()) {
			return (TSource*)it->second;
		}

		return nullptr;
	}

private:
	Signature signature_;
	std::map<ComponentId, void*> components_map_;

};

} // namespace ecs
