#include "pch.h"
#include "Utils.h"
#include "../Ash/Entity.h"

namespace ash_tests {

TEST(Entity, Contain_EmptyEntity_False) {
	ash::Entity entity;

	EXPECT_FALSE(entity.Contain<TestComponent<0>>());
}

TEST(Entity, Add_AddComponent_EntityContainsThisComponent) {
	ash::Entity entity;
	
	entity.Add<TestComponent<0>>();

	EXPECT_TRUE(entity.Contain<TestComponent<0>>());
}

TEST(Entity, Remove_AddAndRemoveComponent_EntityNotContainsThisComponent) {
	ash::Entity entity;

	entity.Add<TestComponent<0>>();
	entity.Remove<TestComponent<0>>();

	EXPECT_FALSE(entity.Contain<TestComponent<0>>());
}

TEST(Entity, Get_AddAndGetComponent_ComponentReceived) {
	ash::Entity entity;

	entity.Add<TestComponent<0>>();
	auto component = entity.Get<TestComponent<0>>();
}

TEST(Entity, Get_GetNotAddedComponent_AssertionFail) {
	ash::Entity entity;

	EXPECT_DEATH(entity.Get<TestComponent<0>>(), "");
}

} // namespace ash_tests