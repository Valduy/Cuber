#pragma once

#include "../ECS/Entity.h"

class RotationComponent : public ecs::Entity::ComponentBase {
public:
	float speed;
	DirectX::SimpleMath::Vector3 axis;
};