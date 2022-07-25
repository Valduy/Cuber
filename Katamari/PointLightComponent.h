#pragma once

#include <SimpleMath.h>

#include "../Ash/Entity.h"
#include "../GraphicEngine/ConstantBuffer.h"

class PointLightComponent : public ash::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector3 view_position;
	DirectX::SimpleMath::Vector3 light_position;
	DirectX::SimpleMath::Vector3 light_color;
	float fade_radius;
	graph::ConstantBuffer light_data_buffer;
};