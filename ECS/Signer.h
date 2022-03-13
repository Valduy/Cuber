#pragma once

#include "BitIdentifier.h"

namespace ecs {
	
class Signer {
public:
	using Signature = BitIdentifier::Id;

	template<typename Last>
	static Signature GetSignature() {
		return BitIdentifier::GetId<Last>();
	}

	template<typename First, typename Second, typename... Rest>
	static Signature GetSignature() {
		static Signature signature = BitIdentifier::GetId<First>() | GetSignature<Second, Rest...>();
		return signature;
	}

	static bool IsMatch(Signature signature, Signature sample) {
		return (signature & sample) == sample;
	}

private:
	Signer() = default;
};

} // namespace ecs