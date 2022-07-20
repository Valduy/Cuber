#include "pch.h"
#include "../Ash/EntityManager.h"

namespace ash_tests {

template<size_t value>
class TestComponent : public ash::Entity::ComponentBase {};

TEST(EntityManager, DeleteRemoveAndFilterEntities) {
	size_t counter = 0;
	ash::EntityManager manager;

	auto& entity0 = manager.Create();
	auto& entity1 = manager.Create();
	auto& entity2 = manager.Create();

	// No one entity contains TestComponent<0>.
	counter = 0;
	for (auto [entity, component0] : manager.Filter<TestComponent<0>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 0);

	entity0.Add<TestComponent<0>>();
	entity1.Add<TestComponent<0>>();
	entity1.Add<TestComponent<1>>();
	entity2.Add<TestComponent<1>>();

	// Some entities contains TestComponent<0>, but invalidation isn't called.
	counter = 0;
	for (auto [entity, component] : manager.Filter<TestComponent<0>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 0);

	// This is new filter and all necessary entities should be added to this filter initially.
	counter = 0;
	for (auto [entity, component] : manager.Filter<TestComponent<1>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 2);

	manager.Invalidate();

	// Entities with TestComponent<0> should be filtered after invalidation.
	counter = 0;
	for (auto [entity, component] : manager.Filter<TestComponent<0>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 2);

	// Check, that TestComponent<1> filter works fine.
	counter = 0;
	for (auto [entity, component] : manager.Filter<TestComponent<1>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 2);

	entity0.Remove<TestComponent<0>>();

	// TestComponent<0> entities become less.
	counter = 0;
	for (auto [entity, component] : manager.Filter<TestComponent<0>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 1);

	// TestComponent<1> entities count is the same.
	counter = 0;
	for (auto [entity, component] : manager.Filter<TestComponent<1>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 2);

	manager.Destroy(entity1);
	manager.Invalidate();

	// TestComponent<0> entities are absent.
	counter = 0;
	for (auto [entity, component] : manager.Filter<TestComponent<0>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 0);

	// Only one TestComponent<1> entity present.
	counter = 0;
	for (auto [entity, component] : manager.Filter<TestComponent<1>>()) {
		counter += 1;
	}
	ASSERT_EQ(counter, 1);
}

} // namespace ash_tests