#pragma once

#include "../ECS/Entity.h"

class RestartComponent : public ecs::Entity::ComponentBase {
public:
	float x;
	float y;
	float delay;
};