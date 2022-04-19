#pragma once

#include <SimpleMath.h>

#include "../ECS/Entity.h"
#include "../GraphicEngine/ShadowMap.h"

class DirectionLightComponent : public ecs::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector3 light_direction;
	DirectX::SimpleMath::Vector3 light_color;
	DirectX::SimpleMath::Matrix view_matrix;
	DirectX::SimpleMath::Matrix projection_matrix;
	graph::ShadowMap shadow_map;

	DirectX::SimpleMath::Matrix GetLightMatrix() const {
		return view_matrix * projection_matrix;
	}
};