#pragma once

#include "ICallable.h"

namespace ecs {

template<typename TObserver, typename TArgs>
class Callback : public ICallable<TArgs> {
public:
	using Action = void (TObserver::*)(TArgs);

	Callback(TObserver observer, Action action)
		: observer_(observer)
		, action_(action)
	{}

	Call(TArgs args) override {
		observer_->*action_(args);
	}

private:
	TObserver* observer_;
	Action action_;

};

} // namespace ecs