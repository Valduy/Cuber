#pragma once

#include <SimpleMath.h>

#include "../Ash/Entity.h"
#include "../GraphicEngine/ShadowMap.h"

class DirectionLightComponent : public ash::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector3 light_direction;
	DirectX::SimpleMath::Vector3 light_color;
	DirectX::SimpleMath::Matrix view_matrix;
	DirectX::SimpleMath::Matrix projection_matrix;
	graph::ShadowMap shadow_map;
	graph::ConstantBuffer light_data_buffer;

	DirectX::SimpleMath::Matrix GetLightMatrix() const {
		return view_matrix * projection_matrix;
	}
};