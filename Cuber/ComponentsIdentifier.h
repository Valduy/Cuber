#pragma once

#include <cstdint>

namespace ecs {

using ComponentId = uint64_t;

class ComponentsIdentifier {
public:
	template<typename TComponent>
	static ComponentId GetComponentId() {
		static ComponentId component_id = GetNextComponentId();
		return component_id;
	}

private:
	static ComponentId current_component_id_;

	ComponentsIdentifier()
	{}

	static ComponentId GetNextComponentId() {
		ComponentId component_id = current_component_id_;
		current_component_id_ <<= 1;
		return component_id;
	}
};

ComponentId ComponentsIdentifier::current_component_id_ = 1;

} // namespace ecs