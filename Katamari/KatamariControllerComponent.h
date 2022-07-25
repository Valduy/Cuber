#pragma once

#include "KatamariControllerComponent.h"
#include "../Ash/Entity.h"

class KatamariControllerComponent : public ash::Entity::ComponentBase {
public:	
	ash::Entity& body;
	float speed;
	float angle_speed;

	KatamariControllerComponent(ash::Entity& body, float speed, float angle_speed)
		: body(body)
		, speed(speed)
		, angle_speed(angle_speed)
	{}
};