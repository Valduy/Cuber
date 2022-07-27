#include "pch.h"
#include "Utils.h"
#include "../Ash/Filter.h"

namespace ash_tests {

class FilterEntitiesFixture : public testing::TestWithParam<size_t> {};

TEST_P(FilterEntitiesFixture, Filter_AddEntitiesToEmptyFamily_IterateOverAllEntities) {
	auto entities = CreateEntities(GetParam());
	ash::Family<> family;
	ash::Filter filter(family);
	size_t count = 0;

	for (auto it : entities) {
		family.TryAddEntity(*it);
	}

	for (auto& node : filter) {
		count += 1;
	}

	EXPECT_EQ(count, entities.size());
	DeleteEntities(&entities);
}

TEST_P(FilterEntitiesFixture, Filter_AddEntitiesWithNecessaryComponentsToFamilyWithComponents_IterateOverAllEntities) {
	auto entities = CreateEntities(GetParam());
	ComponentAdder<TestComponent<0>, TestComponent<1>>::Add(entities.begin(), entities.end());
	ash::Family<TestComponent<0>, TestComponent<1>> family;
	ash::Filter filter(family);
	size_t count = 0;

	for (auto it : entities) {
		family.TryAddEntity(*it);
	}

	for (auto& node : filter) {
		count += 1;
	}

	EXPECT_EQ(count, entities.size());
	DeleteEntities(&entities);
}

INSTANTIATE_TEST_CASE_P(
	Filter,
	FilterEntitiesFixture,
	testing::Values(0, 1, 2, 3, 10), );

} // namespace ash_tests
