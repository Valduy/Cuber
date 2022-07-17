#include <array>
#include <algorithm>
#include "pch.h"
#include "Utils.h"
#include "../Ash/Signature.h"

namespace ash_tests {

TEST(Signature, kIds_EmptySignature_kIdsSizeIs0) {
	const auto& signature = ash::Signature<>::kIds;

	EXPECT_EQ(signature.size(), 0);
}

TEST(Signature, kIds_SomeTypes_SignatureIdsCorrespondToTypes) {
	std::array<ash::Type::Id, 3> ids;
	const auto& signature = ash::Signature<TestComponent<0>, TestComponent<1>, TestComponent<2>>::kIds;

	ids[0] = ash::Type::GetId<TestComponent<0>>();
	ids[1] = ash::Type::GetId<TestComponent<1>>();
	ids[2] = ash::Type::GetId<TestComponent<2>>();
	std::sort(ids.begin(), ids.end());
	
	EXPECT_TRUE(std::equal(ids.begin(), ids.end(), signature.begin()));
}

TEST(Signature, kIds_SomeTypes_SameSignaturesForSameTypes) {
	const auto& signature1 = ash::Signature<TestComponent<0>, TestComponent<1>, TestComponent<2>>::kIds;
	const auto& signature2 = ash::Signature<TestComponent<0>, TestComponent<1>, TestComponent<2>>::kIds;

	EXPECT_TRUE(std::equal(signature1.begin(), signature1.end(), signature2.begin()));
}

TEST(Signature, kIds_SomeTypes_OrderIsNotMatterForSameSignature) {
	const auto& signature1 = ash::Signature<TestComponent<0>, TestComponent<1>, TestComponent<2>>::kIds;
	const auto& signature2 = ash::Signature<TestComponent<2>, TestComponent<1>, TestComponent<0>>::kIds;

	EXPECT_TRUE(std::equal(signature1.begin(), signature1.end(), signature2.begin()));
}

TEST(Signature, kIds_SomeTypes_DifferentSignaturesForDifferentTypes) {
	const auto& signature1 = ash::Signature<TestComponent<0>, TestComponent<1>, TestComponent<2>>::kIds;
	const auto& signature2 = ash::Signature<TestComponent<1>, TestComponent<2>, TestComponent<3>>::kIds;

	EXPECT_FALSE(std::equal(signature1.begin(), signature1.end(), signature2.begin()));
}

} // namespace ash_tests