#pragma once

#include "../ECS/Entity.h"

class BoundingBoxComponent : public ecs::Entity::ComponentBase {
public:
	float width;
	float height;
};