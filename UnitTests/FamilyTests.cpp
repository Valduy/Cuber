#include "pch.h"
#include <array>
#include <vector>
#include "Utils.h"
#include "../Ash/Family.h"

namespace ash_tests {

class FamilyEntitiesFixture : public testing::TestWithParam<size_t> {};

TEST_P(FamilyEntitiesFixture, TryAddEntity_AddEntityToEmptyFamily_AllEntityAddedToFamily) {
	auto entities = CreateEntities(GetParam());
	ash::Family<> family;
	ComponentAdder<TestComponent<0>, TestComponent<1>>::Add(entities.begin(), entities.end());
	size_t count = 0;
	
	for (const auto it : entities) {
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

	for (const auto it : entities) {
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

	for (const auto it : entities) {
		family.TryAddEntity(*it);
	}

	for (auto& node : family) {
		count += 1;
	}

	EXPECT_EQ(count, 0);
	DeleteEntities(&entities);
}

TEST_P(FamilyEntitiesFixture, OnComponentAddedToEntity_NotCorrectComponents_EntitiesNotAdded) {
	auto entities = CreateEntities(GetParam());
	ash::Family<TestComponent<2>> family;
	size_t count = 0;

	for (const auto it : entities) {
		it->Add<TestComponent<0>>();
		const ash::Type::Id id = ash::Type::GetId<TestComponent<0>>();
		family.OnComponentAddedToEntity(*it, id);
	}

	for (auto& node : family) {
		count += 1;
	}

	EXPECT_EQ(count, 0);
	DeleteEntities(&entities);
}

TEST_P(FamilyEntitiesFixture, OnComponentAddedToEntity_CorrectComponents_EntitiesAdded) {
	auto entities = CreateEntities(GetParam());
	ash::Family<TestComponent<0>> family;
	size_t count = 0;

	for (const auto it : entities) {
		it->Add<TestComponent<0>>();
		const ash::Type::Id id = ash::Type::GetId<TestComponent<0>>();
		family.OnComponentAddedToEntity(*it, id);
	}

	for (auto& node : family) {
		count += 1;
	}

	EXPECT_EQ(count, entities.size());
	DeleteEntities(&entities);
}

TEST_P(FamilyEntitiesFixture, OnComponentRemovedFromEntity_CorrectComponents_EntitiesRemoved) {
	auto entities = CreateEntities(GetParam());
	ash::Family<TestComponent<0>> family;
	ComponentAdder<TestComponent<0>>::Add(entities.begin(), entities.end());
	size_t count = 0;

	for (const auto it : entities) {
		family.TryAddEntity(*it);
	}

	for (const auto it : entities) {
		it->Remove<TestComponent<0>>();
		const ash::Type::Id id = ash::Type::GetId<TestComponent<0>>();
		family.OnComponentRemovedFromEntity(*it, id);
	}

	for (auto& node : family) {
		count += 1;
	}

	EXPECT_EQ(count, 0);
	DeleteEntities(&entities);
}

TEST_P(FamilyEntitiesFixture, OnComponentRemovedFromEntity_RemoveComponentsWhenIterate_IterateAllEntities) {
	auto entities = CreateEntities(GetParam());
	ash::Family<TestComponent<0>> family;
	ComponentAdder<TestComponent<0>>::Add(entities.begin(), entities.end());
	size_t count = 0;

	for (const auto it : entities) {
		family.TryAddEntity(*it);
	}
	
	for (auto& [entity, component] : family) {
		entity.Remove<TestComponent<0>>();
		const ash::Type::Id id = ash::Type::GetId<TestComponent<0>>();
		family.OnComponentRemovedFromEntity(entity, id);
		count += 1;
	}

	EXPECT_EQ(count, entities.size());
	DeleteEntities(&entities);
}

TEST_P(FamilyEntitiesFixture, RemoveEntity_RemoveAllAddedEntities_FamilyIsEmpty) {
	auto entities = CreateEntities(GetParam());
	ash::Family<TestComponent<0>> family;
	ComponentAdder<TestComponent<0>>::Add(entities.begin(), entities.end());
	size_t count = 0;

	for (const auto it : entities) {
		family.TryAddEntity(*it);
	}

	for (const auto it : entities) {
		family.RemoveEntity(*it);
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
	testing::Values(0, 1, 2, 3, 10),);

} // namespace ash_tests