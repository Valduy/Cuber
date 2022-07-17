#pragma once
#include "Delegate.h"

namespace utils {

template<typename... TParams>
class Event {
public:
	Event(Delegate<TParams...>& delegate)
		: delegate_(delegate)
	{}

	template<typename TObject>
	bool Attach(TObject& object, EventHandler<TObject, TParams...>::TMethod method) const {
		return delegate_.Attach(object, method);
	}

	template<typename TObject>
	bool Detach(TObject& object, EventHandler<TObject, TParams...>::TMethod method) const {
		return delegate_.Detach(object, method);
	}

private:
	Delegate<TParams...>& delegate_;
};

} // namespace utils