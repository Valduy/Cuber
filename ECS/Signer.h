#pragma once

#include "ComponentsIdentifier.h"

namespace ecs {

using Signature = ComponentId;

class Signer {
public:
	template<typename Last>
	static Signature GetSignature() {
		return ComponentsIdentifier::GetComponentId<Last>();
	}

	template<typename First, typename Second, typename... Rest>
	static Signature GetSignature() {
		Signature signature = ComponentsIdentifier::GetComponentId<First>();
		signature |= GetSignature<Second, Rest...>();
		return signature;
	}

	static bool IsMatch(Signature signature, Signature sample) {
		return (signature & sample) == sample;
	}

private:
	Signer() = default;
};

} // namespace ecs