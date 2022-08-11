#pragma once

#include <system_error>

enum class CoreErrors {
	kSuccess = 0,
	kFailToLoadResource,
	kResourceIsInvalid,
	kInternalError,
};

namespace std {

	template<>
	struct is_error_code_enum<CoreErrors> : true_type {};

} // namespace std;

std::error_code make_error_code(CoreErrors e);