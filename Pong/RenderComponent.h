#pragma once

#include "../ECS/Entity.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/IndexBuffer.h"
#include "../GraphicEngine/ConstantBuffer.h"

class RenderComponent : public ecs::Entity::ComponentBase {
public:
	std::vector<DirectX::SimpleMath::Vector4> vertices;
	graph::VertexBuffer vertex_buffer;
	graph::IndexBuffer index_buffer;
	graph::ConstantBuffer transform_buffer;

	RenderComponent(
		std::vector<DirectX::SimpleMath::Vector4> vertices,
		graph::VertexBuffer vertex_buffer,
		graph::IndexBuffer index_buffer,
		graph::ConstantBuffer constant_buffer)
		: vertices(vertices)
		, vertex_buffer(vertex_buffer)
		, index_buffer(index_buffer)
		, transform_buffer(constant_buffer)
	{}
};
