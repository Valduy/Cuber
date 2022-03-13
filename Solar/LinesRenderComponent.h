#pragma once

#include "../ECS/IComponent.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/ConstantBuffer.h"

class LinesRenderComponent : public ecs::IComponent {
public:
	graph::VertexBuffer vertex_buffer;
	graph::ConstantBuffer matrix_const_buffer;
	graph::ConstantBuffer color_const_buffer;

	LinesRenderComponent(
		graph::VertexBuffer vertex_buffer,
		graph::ConstantBuffer matrix_const_buffer,
		graph::ConstantBuffer color_const_buffer)
		: vertex_buffer(vertex_buffer)
		, matrix_const_buffer(matrix_const_buffer)
		, color_const_buffer(color_const_buffer)
	{}
};