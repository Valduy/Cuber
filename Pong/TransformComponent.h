#pragma once

#include "../Ash/Entity.h"

class TransformComponent : public ash::Entity::ComponentBase {
public:
	float x;
	float y;
};