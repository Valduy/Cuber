#pragma once

#include "../ECS/Entity.h"

class VelocityComponent : public ecs::Entity::ComponentBase {
public:
	float x;
	float y;
};