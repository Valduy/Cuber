#pragma once

#include "../ECS/IComponent.h"

class BoundingBoxComponent : public ecs::IComponent {
public:
	float width;
	float height;
};