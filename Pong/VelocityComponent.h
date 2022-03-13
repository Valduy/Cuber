#pragma once

#include "../ECS/ComponentBase.h"

class VelocityComponent : public ecs::ComponentBase {
public:
	float x;
	float y;
};