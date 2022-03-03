#pragma once

#include "../ECS/IComponent.h"

class VelocityComponent : public ecs::IComponent {
public:
	float x;
	float y;
};