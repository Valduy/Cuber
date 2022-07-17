#pragma once

namespace utils {

template<typename... TParams>
class IEventHandler {
public:
	virtual ~IEventHandler() = default;
	virtual void Call(TParams... params) = 0;
	virtual bool IsEqual(const IEventHandler& other) const = 0;
};

} // namespace utils