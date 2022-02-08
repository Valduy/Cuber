#include "pch.h"
#include "../ECS/Signer.h"

namespace ecs_tests {

template<int number>
class TestComponent {
public:
	const int kNumber = number;
};

TEST(SignerTests, GetSignature_1Type_SignatureForThisType) {
	const auto id = ecs::BitIdentifier::GetId<TestComponent<1>>();
	const auto signature = ecs::Signer::GetSignature<TestComponent<1>>();

	EXPECT_EQ(id, signature);
}

TEST(SignerTests, GetSignature_2Types_SignatureForTheseTypes) {
	const auto id1 = ecs::BitIdentifier::GetId<TestComponent<1>>();
	const auto id2 = ecs::BitIdentifier::GetId<TestComponent<2>>();
	const auto id3 = ecs::BitIdentifier::GetId<TestComponent<3>>();
	const auto signature = ecs::Signer::GetSignature<TestComponent<1>, TestComponent<3>>();

	const auto sample = id1 | id3;

	EXPECT_EQ(signature, sample);
}

TEST(SignerTests, IsMatch_IdealSignature_True) {
	const auto id1 = ecs::BitIdentifier::GetId<TestComponent<1>>();
	const auto id2 = ecs::BitIdentifier::GetId<TestComponent<2>>();
	const auto id3 = ecs::BitIdentifier::GetId<TestComponent<3>>();
	const auto signature = ecs::Signer::GetSignature<TestComponent<1>, TestComponent<3>>();

	const auto sample = id1 | id3;

	EXPECT_TRUE(ecs::Signer::IsMatch(signature, sample));
}

TEST(SignerTests, IsMatch_ComplexSignature_True) {
	const auto id1 = ecs::BitIdentifier::GetId<TestComponent<1>>();
	const auto id2 = ecs::BitIdentifier::GetId<TestComponent<2>>();
	const auto id3 = ecs::BitIdentifier::GetId<TestComponent<3>>();
	const auto signature = ecs::Signer::GetSignature<TestComponent<1>, TestComponent<2>, TestComponent<3>>();

	const auto sample = id1 | id3;

	EXPECT_TRUE(ecs::Signer::IsMatch(signature, sample));
}

TEST(SignerTests, IsMatch_NotSuitableSignature_False) {
	const auto id1 = ecs::BitIdentifier::GetId<TestComponent<1>>();
	const auto id2 = ecs::BitIdentifier::GetId<TestComponent<2>>();
	const auto id3 = ecs::BitIdentifier::GetId<TestComponent<3>>();
	const auto signature = ecs::Signer::GetSignature<TestComponent<1>, TestComponent<2>>();

	const auto sample = id1 | id3;

	EXPECT_FALSE(ecs::Signer::IsMatch(signature, sample));
}

} // namespace ecs_tests