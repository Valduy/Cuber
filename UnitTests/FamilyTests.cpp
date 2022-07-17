#include <array>
#include <vector>
#include "pch.h"
#include "Utils.h"
#include "../Ash/Family.h"

namespace ash_tests {

// remove entity
// remove entity component
// add entity component

class FamilyEntitiesFixture : public testing::TestWithParam<size_t> {};

TEST_P(FamilyEntitiesFixture, TryAddEntity_AddEntityToEmptyFamily_AllEntityAddedToFamily) {
	auto entities = CreateEntities(GetParam());
	ash::Family<> family;
	ComponentAdder<TestComponent<0>, TestComponent<1>>::Add(entities.begin(), entities.end());
	size_t count = 0;
	
	for (auto it : entities) {
		family.TryAddEntity(*it);
	}

	for (auto node : family) {
		count += 1;
	}

	EXPECT_EQ(count, entities.size());
	DeleteEntities(&entities);
}

TEST_P(FamilyEntitiesFixture, TryAddEntity_AddEntityWithNecessaryComponents_EntityAddedToFamily) {
	auto entities = CreateEntities(GetParam());
	ash::Family<TestComponent<0>, TestComponent<1>> family;
	ComponentAdder<TestComponent<0>, TestComponent<1>>::Add(entities.begin(), entities.end());
	size_t count = 0;

	for (auto it : entities) {
		family.TryAddEntity(*it);
	}

	for (auto& node : family) {
		count += 1;
	}

	EXPECT_EQ(count, entities.size());
	DeleteEntities(&entities);
}

TEST_P(FamilyEntitiesFixture, TryAddEntity_AddEntityWithoutNecessaryComponents_EntityNotAddedToFamily) {
	auto entities = CreateEntities(GetParam());
	ash::Family<TestComponent<2>> family;
	ComponentAdder<TestComponent<0>, TestComponent<1>>::Add(entities.begin(), entities.end());
	size_t count = 0;

	for (auto it : entities) {
		family.TryAddEntity(*it);
	}

	for (auto& node : family) {
		count += 1;
	}

	EXPECT_EQ(count, 0);
	DeleteEntities(&entities);
}

INSTANTIATE_TEST_CASE_P(
	Family,
	FamilyEntitiesFixture,
	testing::Values(0, 1, 2, 3),);

} // namespace ash_tests