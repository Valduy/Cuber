#pragma once

#include <iterator>

namespace ecs {

template<typename TSubject>
class Generator {
public:
	Generator(TSubject first, TSubject last)
		: first_(first)
		, last_(last)
		, temp_(first_)
	{}

	TSubject& GetCurrent() {
		return *temp_;
	}

	bool MoveNext() {
		std::advance(temp_, 1);
		return temp_ != last_;
	}

	void Reset() {
		temp_ = first_;
	}

private:
	TSubject first_;
	TSubject last_;
	TSubject temp_;
};

} // namespace ecs