#pragma once

#include "../ECS/IComponent.h"

class TransformComponent : public ecs::IComponent {
public:
	float x;
	float y;
};