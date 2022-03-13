#pragma once

#include "../ECS/ComponentBase.h"

class BoundingBoxComponent : public ecs::ComponentBase {
public:
	float width;
	float height;
};