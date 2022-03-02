#pragma once

#include "../ECS/IComponent.h"

class BoundingBoxComponent : ecs::IComponent {
public:
	float left;
	float top;
	float right;
	float bottom;
};