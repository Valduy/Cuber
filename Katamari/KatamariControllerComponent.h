#pragma once

#include "KatamariControllerComponent.h"
#include "../ECS/Entity.h"

class KatamariControllerComponent : public ecs::Entity::ComponentBase {
public:	
	ecs::Entity& body;
	float speed;

	KatamariControllerComponent(ecs::Entity& body, float speed)
		: body(body)
		, speed(speed)
	{}
};