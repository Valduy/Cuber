#pragma once

#include <vector>
#include <map>
#include "FilterFactory.h"
#include "SystemsIdentifier.h"
#include "SystemBase.h"

namespace ecs {

class Engine {
public:	
	using EntityIterator = std::vector<Entity*>::iterator;

	FilterBuilder<EntityIterator> FromEntities() {
		Generator<EntityIterator> generator(entities_.begin(), entities_.end());
		return FilterBuilder<EntityIterator>(generator);
	}

	void Update(float dt) {

	}

	template<typename TSubject>
	void AddSystem(SystemBase::Priority priority) {
		
	}
	
	//template<typename TSubject>
	//bool RemoveSystem() {
	//	
	//}

	//template<typename TSubject>
	//TSubject* GetSystem() {

	//}

private:
	std::vector<Entity*> entities_;
	std::map<SystemId, SystemBase*> systems_;
	

};

} // namespace ecs

