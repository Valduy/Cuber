#pragma once

#include "../Ash/Entity.h"

class SpeedComponent : public ash::Entity::ComponentBase {
public:
	float speed;
};