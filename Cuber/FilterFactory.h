#pragma once

#include "Signer.h"
#include "Filter.h"

namespace ecs {

template<typename T>
class FilterBuilder {
public:
	FilterBuilder(Generator<T> generator) 
		: generator_(generator)
		, predicate_([](T e) { return true; })
	{}

	template<typename First, typename... Rest>
	FilterBuilder Where() {
		Signature sample = Signer::GetSignature<First, Rest...>();		
		predicate_ = [p = predicate_](T e)
		{
			return p(e) && Signer::IsMatch(e->GetSignature(), sample);
		};
		
		return *this;
	}

	template<typename First, typename... Rest>
	FilterBuilder Except() {
		Signature sample = Signer::GetSignature<First, Rest...>();
		predicate_ = [p = predicate_](T e)
		{
			return p(e) && !Signer::IsMatch(e->GetSignature(), sample);
		};

		return *this;
	}

	Filter<T> Get() {
		return Filter<T>(generator_, predicate_);
	}

private:
	Generator<T> generator_;
	std::function<bool(T)> predicate_;

};

} // namespace ecs