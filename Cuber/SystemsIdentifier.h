#pragma once

#include <cstdint>

namespace ecs {

using SystemId = uint32_t;

class SystemsIdentifier {
public:
	template<typename TSystem>
	static SystemId GetSystemId() {
		static SystemId system_id = GetNextSystemId();
		return system_id;
	}

private:
	static SystemId current_system_id_;

	SystemsIdentifier()
	{}

	static SystemId GetNextSystemId() {
		SystemId system_id = current_system_id_;
		current_system_id_ += 1;
		return system_id;
	}
};

SystemId SystemsIdentifier::current_system_id_ = 1;

} // namespace ecs