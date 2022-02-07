#pragma once

namespace ecs {

template<typename TArgs>
class ICallable {
public:
	virtual ~ICallable() {}
	virtual void Call(TArgs args) = 0;
};

}