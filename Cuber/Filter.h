#pragma once

#include <functional>
#include "Entity.h"
#include "Generator.h"

namespace ecs {

template<typename TSubject>
class Filter {
public:
	Filter(Generator<TSubject> generator, std::function<bool(TSubject)> predicate)
		: generator_(generator)
		, predicate_(predicate)
	{}

	TSubject& GetCurrent() {
		return *generator_.GetCurrent();
	}

	bool MoveNext() {
		while (generator_.MoveNext()) {
			if (predicate_(generator_.GetCurrent())) {
				return true;
			}
		}
		
		return false;
	}

	void Reset() {
		generator_.Reset();
	}

private:
	Generator<TSubject> generator_;
	std::function<bool(TSubject)> predicate_;
};

} // namespace ecs