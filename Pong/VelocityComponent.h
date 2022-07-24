#pragma once

#include "../Ash/Entity.h"

class VelocityComponent : public ash::Entity::ComponentBase {
public:
	float x;
	float y;
};