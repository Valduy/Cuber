#pragma once

#include "KatamariControllerComponent.h"
#include "../ECS/Entity.h"

class KatamariControllerComponent : public ecs::Entity::ComponentBase {
public:	
	ecs::Entity& body;
	float speed;
	float angle_speed;

	KatamariControllerComponent(ecs::Entity& body, float speed, float angle_speed)
		: body(body)
		, speed(speed)
		, angle_speed(angle_speed)
	{}
};