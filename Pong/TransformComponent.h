#pragma once

#include "../ECS/ComponentBase.h"

class TransformComponent : public ecs::ComponentBase {
public:
	float x;
	float y;
};