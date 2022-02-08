#pragma once

#include <cstdint>
#include <cassert>

namespace ecs {

using ComponentId = uint64_t;

class ComponentsIdentifier {
public:
	static const ComponentId kOverflowId;

	template<typename TComponent>
	static ComponentId GetComponentId() {
		static ComponentId component_id = GetNextComponentId();
		assert(component_id != kOverflowId && "Component's id overflow.");
		return component_id;
	}

private:
	static ComponentId current_component_id_;

	ComponentsIdentifier() = default;

	static ComponentId GetNextComponentId() {
		const ComponentId component_id = current_component_id_;
		current_component_id_ <<= 1;
		return component_id;
	}
};

ComponentId ComponentsIdentifier::current_component_id_ = 1;

} // namespace ecs