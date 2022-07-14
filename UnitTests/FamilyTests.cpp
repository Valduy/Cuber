#include <array>
#include <vector>
#include "pch.h"
#include "../Ash/Family.h"

namespace ash_tests {

// empty family
// try add correct entities
// try add not correct entities
//

template<size_t value>
class TestComponent : public ash::Entity::ComponentBase {};

class EntitiesFixture : public testing::TestWithParam<size_t> {};

template<typename... Args>
struct ComponentAdder;

template<typename Head, typename... Tail>
struct ComponentAdder<Head, Tail...> {
	static void Add(ash::Entity& entity) {
		entity.Add<Head>();
		ComponentAdder<Tail...>::Add(entity);
	}

	template<typename T>
	static void Add(T first, T last) {
		for (auto it = first; it != last; ++it) {
			auto entity = *it;
			ComponentAdder<Head, Tail...>::Add(*entity);
		}
	}
};

template<>
struct ComponentAdder<> {
	static void Add(ash::Entity& entity) {}
};

std::vector<ash::Entity*> CreateEntities(size_t size) {
	std::vector<ash::Entity*> result;

	for (size_t i = 0; i < size; ++i) {
		auto entity = new ash::Entity();
		result.push_back(entity);
	}

	return result;
}

TEST_P(EntitiesFixture, TryAddEntity_AddEntityToEmptyFamily_AllEntityAddedToFamily) {
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
}

TEST_P(EntitiesFixture, TryAddEntity_AddEntityWithNecessaryComponents_EntityAddedToFamily) {
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
}

TEST_P(EntitiesFixture, TryAddEntity_AddEntityWithoutNecessaryComponents_EntityNotAddedToFamily) {
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
}

INSTANTIATE_TEST_CASE_P(
	Family,
	EntitiesFixture,
	testing::Values(0, 1, 2, 3),);

} // namespace ash_tests