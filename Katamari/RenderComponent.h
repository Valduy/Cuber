#pragma once

#include "../ECS/Entity.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/IndexBuffer.h"
#include "../GraphicEngine/ConstantBuffer.h"
#include "../GraphicEngine/Texture.h"

class MeshBuffers {
public:
	graph::VertexBuffer vertex_buffer;
	graph::IndexBuffer index_buffer;	

	MeshBuffers(
		graph::VertexBuffer vertex_buffer,
		graph::IndexBuffer index_buffer)
		: vertex_buffer(vertex_buffer)
		, index_buffer(index_buffer)
	{}
};

class RenderComponent : public ecs::Entity::ComponentBase {
public:
	std::vector<MeshBuffers> model_buffers;
	graph::ConstantBuffer transform_buffer;
	graph::ConstantBuffer material_buffer;
	graph::Texture texture;

	RenderComponent(
		std::vector<MeshBuffers> model_buffers,
		graph::ConstantBuffer transform_buffer,
		graph::ConstantBuffer material_buffer,
		graph::Texture texture)
		: model_buffers(model_buffers)
		, transform_buffer(transform_buffer)
		, material_buffer(material_buffer)
		, texture(texture)
	{}
};