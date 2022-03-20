#pragma once

#include "../ECS/Entity.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/IndexBuffer.h"
#include "../GraphicEngine/ConstantBuffer.h"

class MeshBuffers {
public:
	graph::VertexBuffer vertex_buffers;
	graph::IndexBuffer index_buffers;

	MeshBuffers(
		graph::VertexBuffer vertex_buffers,
		graph::IndexBuffer index_buffers)
		: vertex_buffers(vertex_buffers)
		, index_buffers(index_buffers)
	{}
};

class RenderComponent : public ecs::Entity::ComponentBase {
public:
	std::vector<MeshBuffers> model_buffers;
	graph::ConstantBuffer constant_buffer;

	RenderComponent(
		std::vector<MeshBuffers> model_buffers,
		graph::ConstantBuffer constant_buffer)
		: model_buffers(model_buffers)
		, constant_buffer(constant_buffer)
	{}
};