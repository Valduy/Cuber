#pragma once

#include "../ECS/Entity.h"

class SpeedComponent : public ecs::Entity::ComponentBase {
public:
	float speed;
};