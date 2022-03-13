#pragma once

#include "../ECS/ComponentBase.h"

class RestartComponent : public ecs::ComponentBase{
public:
	float x;
	float y;
	float delay;
};