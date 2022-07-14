#pragma once

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace ash {

class Type {
public:
	using Id = uint32_t;

	template<typename T>
	static Id GetId() {
		static Id component_id = GetNextId();	
		return component_id;
	}

private:
	static Id current_id_;

	Type() = default;

	static Id GetNextId() {
		assert(current_id_ != 0 && "Type's id overflow.");
		const Id id = current_id_;
		current_id_ += 1;
		return id;
	}
};

} // namespace ash