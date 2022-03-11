#pragma once

#include "../ECS/IComponent.h"

class RotationComponent : public ecs::IComponent {
public:
	float speed;
	DirectX::SimpleMath::Vector3 axis;
};