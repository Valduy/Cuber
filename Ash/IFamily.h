#pragma once

#include "Entity.h"

namespace ash {

class IFamily {
public:
	virtual ~IFamily() = default;

	virtual void Invalidate() = 0;

	// Method try to add an entity into family. If this entity contains all required components,
	// the family store it and create a node. Otherwise, the family ignores this entity.
	// Note: this method should only be called from the entity manager when the family is created, to initialize
	// that family by initial entities.
	virtual bool TryAddEntity(Entity& entity) = 0;

	// Method try to remove entity from family. If entity was contained in the family, method return true. Otherwise,
	// method return false.
	// Note: this method should be called from the entity manager on entity deletation.
	virtual bool RemoveEntity(Entity& entity) = 0;

	virtual void OnComponentAddedToEntity(Entity& entity, Type::Id id) = 0;
	virtual void OnComponentRemovedFromEntity(Entity& entity, Type::Id id) = 0;
};

} // namespace ash