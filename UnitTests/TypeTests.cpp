#include "pch.h"
#include "Utils.h"
#include "../Ash/Type.h"

namespace ash_tests {

TEST(Type, GetId_GetIdFor2Types_UniqueIdsForEachType) {
	const auto id1 = ash::Type::GetId<TestComponent<0>>();
	const auto id2 = ash::Type::GetId<TestComponent<1>>();

	EXPECT_NE(id1, id2);
}

TEST(Type, GetId_GetIdFor2Times_SameIdForEachTime) {
	const auto id1 = ash::Type::GetId<TestComponent<0>>();
	const auto id2 = ash::Type::GetId<TestComponent<0>>();

	EXPECT_EQ(id1, id2);
}


} // namespace ash_tests

