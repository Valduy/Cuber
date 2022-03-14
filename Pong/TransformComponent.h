#pragma once

#include "../ECS/Entity.h"

class TransformComponent : public ecs::Entity::ComponentBase {
public:
	float x;
	float y;
};