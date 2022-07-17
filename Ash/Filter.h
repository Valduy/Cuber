#pragma once

#include "Family.h"

namespace ash {

template<typename... Args>
class Filter {
public:
	Filter(Family<Args...>& family)
		: family_(family)
	{}

	Family<Args...>::Iterator GetIterator() {
		return family_.GetIterator();
	}

	Family<Args...>::Iterator begin() {
		return family_.begin();
	}

	Family<Args...>::Iterator end() {
		return family_.end();
	}

private:
	Family<Args...>& family_;
};

} // namespace ash