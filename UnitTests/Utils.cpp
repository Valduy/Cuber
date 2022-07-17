#include "pch.h"
#include "Utils.h"

namespace ash_tests {

std::vector<ash::Entity*> CreateEntities(size_t size) {
	std::vector<ash::Entity*> result;

	for (size_t i = 0; i < size; ++i) {
		auto entity = new ash::Entity();
		result.push_back(entity);
	}

	return result;
}

void DeleteEntities(std::vector<ash::Entity*>* entities) {
	for (auto e : *entities) {
		delete e;
	}
}

} // namespace ash_tests