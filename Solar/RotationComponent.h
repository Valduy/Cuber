#pragma once

#include "../ECS/ComponentBase.h"

class RotationComponent : public ecs::ComponentBase {
public:
	float speed;
	DirectX::SimpleMath::Vector3 axis;
};