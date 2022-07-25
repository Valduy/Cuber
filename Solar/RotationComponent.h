#pragma once

#include "../Ash/Entity.h"

class RotationComponent : public ash::Entity::ComponentBase {
public:
	float speed;
	DirectX::SimpleMath::Vector3 axis;
};