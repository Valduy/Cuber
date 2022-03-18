#pragma once

#include "../ECS/Entity.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/ConstantBuffer.h"

namespace engine {

class LinesRenderComponent : public ecs::Entity::ComponentBase {
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

} // namespace engine
