#pragma once

#include "../ECS/Entity.h"
#include "../GraphicEngine/ConstantBuffer.h"

class ShadowComponent : public ecs::Entity::ComponentBase {
public:
	graph::ConstantBuffer transform_buffer;

	ShadowComponent(graph::ConstantBuffer transform_buffer)
		: transform_buffer(transform_buffer)
	{}
};