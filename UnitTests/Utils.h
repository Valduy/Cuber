#pragma once

#include "../Ash/Entity.h"

namespace ash_tests {

template<size_t value>
class TestComponent : public ash::Entity::ComponentBase {};

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

std::vector<ash::Entity*> CreateEntities(size_t size);
void DeleteEntities(std::vector<ash::Entity*>* entities);

} // namespace ash_tests