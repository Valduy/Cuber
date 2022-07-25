#pragma once

#include "../Ash/Entity.h"
#include "../GraphicEngine/ConstantBuffer.h"

class ShadowComponent : public ash::Entity::ComponentBase {
public:
	graph::ConstantBuffer transform_buffer;

	ShadowComponent(graph::ConstantBuffer transform_buffer)
		: transform_buffer(transform_buffer)
	{}
};