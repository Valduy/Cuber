#include "pch.h"
#include "../ECS/BitIdentifier.h"

namespace ecs_tests {

template<int number>
class TestComponent {
public:
	const int kNumber = number;
};

template<int n>
struct IdConsumer {
	static void ConsumeId() {
		ecs::BitIdentifier::GetId<TestComponent<n>>();
		IdConsumer<n - 1>::ConsumeId();
	}
};

template<>
struct IdConsumer<0> {
	static void ConsumeId() {
		return;
	}
};

TEST(ComponentsIdentifierTests, GetId_GetIdForSingleType_1) {
	const auto id = ecs::BitIdentifier::GetId<TestComponent<1>>();

	EXPECT_EQ(id, 1);
}

TEST(ComponentsIdentifierTests, GetId_GetIdFor2Types_UniqueIdsForEachType) {
	const auto id1 = ecs::BitIdentifier::GetId<TestComponent<1>>();
	const auto id2 = ecs::BitIdentifier::GetId<TestComponent<2>>();

	EXPECT_NE(id1, id2);
}

TEST(ComponentsIdentifierTests, GetId_GetIdFor2Times_SameIdForEachTime) {
	const auto id1 = ecs::BitIdentifier::GetId<TestComponent<1>>();
	const auto id2 = ecs::BitIdentifier::GetId<TestComponent<1>>();

	EXPECT_EQ(id1, id2);
}

TEST(ComponentsIdentifierTests, GetId_IdOverflow_AssertionFail) {
	EXPECT_DEATH(IdConsumer<sizeof(ecs::BitIdentifier::Id) * 8 + 1>::ConsumeId(), "");
}

} // namespace ecs_tests