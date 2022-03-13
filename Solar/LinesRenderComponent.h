#pragma once

#include "../ECS/IComponent.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/ConstantBuffer.h"

class LinesRenderComponent : public ecs::IComponent {
public:
	graph::VertexBuffer vertex_buffer;
	graph::ConstantBuffer constant_buffer;

	LinesRenderComponent(
		graph::VertexBuffer vertex_buffer,
		graph::ConstantBuffer constant_buffer)
		: vertex_buffer(vertex_buffer)
		, constant_buffer(constant_buffer)
	{}
};