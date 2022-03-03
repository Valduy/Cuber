#pragma once

#include "../ECS/IComponent.h"

class RestartComponent : public ecs::IComponent{
public:
	float x;
	float y;
	float delay;
};