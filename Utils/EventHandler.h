#pragma once

#include <cassert>
#include "IEventHandler.h"

namespace utils {

template<typename TObject, typename... TParams>
class EventHandler : public IEventHandler<TParams...> {
public:
	using TMethod = void(TObject::*)(TParams...);
	using MyType = EventHandler<TObject, TParams...>;
	using BaseType = IEventHandler<TParams...>;

	EventHandler(TObject& object, TMethod method)
		: object_(object)
		, method_(method)
	{
		assert(method_ != nullptr && "Method can't be nullptr.");
	}

	void Call(TParams... params) override {
		(object_.*method_)(params...);
	}

	bool IsEqual(const BaseType& other) const override {
		const auto* concrete = dynamic_cast<const MyType*>(&other);
		return concrete != nullptr
			&& &object_ == &(concrete->object_)
			&& method_ == concrete->method_;
	}

private:
	TObject& object_;
	TMethod method_;
};

} // namespace utils