#pragma once

#include <SimpleMath.h>

#include "../ECS/Entity.h"

class DirectionLightComponent : public ecs::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector3 light_direction;
	DirectX::SimpleMath::Vector3 light_color;
};