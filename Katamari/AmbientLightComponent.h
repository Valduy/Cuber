#pragma once

#include <SimpleMath.h>
#include "../ECS/Entity.h"
#include "../GraphicEngine/ConstantBuffer.h"

class AmbientLightComponent : public ecs::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector3 light_color;
	float intensity;
	graph::ConstantBuffer light_data_buffer;
};