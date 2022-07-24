#pragma once

#include "../Ash/Entity.h"

class BoundingBoxComponent : public ash::Entity::ComponentBase {
public:
	float width;
	float height;
};