#pragma once

#include "../Ash/Entity.h"
#include "../GraphicEngine/ConstantBuffer.h"

class ForwardRenderComponent : public ash::Entity::ComponentBase {
public:
	graph::ConstantBuffer light_transform_buffer;
};