#pragma once

#include "../Ash/Entity.h"

class RestartComponent : public ash::Entity::ComponentBase {
public:
	float x;
	float y;
	float delay;
};