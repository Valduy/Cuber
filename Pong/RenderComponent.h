#pragma once

#include <directxmath.h>

#include "../ECS/IComponent.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/IndexBuffer.h"
#include "../GraphicEngine/ConstantBuffer.h"

class RenderComponent : public ecs::IComponent {
public:
	std::vector<DirectX::XMFLOAT4> vertices;
	graph::VertexBuffer vertex_buffer;
	graph::IndexBuffer index_buffer;
	graph::ConstantBuffer constant_buffer;

	RenderComponent(
		std::vector<DirectX::XMFLOAT4> vertices,
		graph::VertexBuffer vertex_buffer,
		graph::IndexBuffer index_buffer,
		graph::ConstantBuffer constant_buffer)
		: vertices(vertices)
		, vertex_buffer(vertex_buffer)
		, index_buffer(index_buffer)
		, constant_buffer(constant_buffer)
	{}
};
