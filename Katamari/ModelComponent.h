#pragma once

#include "../Ash/Entity.h"
#include "../Engine/Model.h"
#include "../GraphicEngine/ConstantBuffer.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/IndexBuffer.h"

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

class ModelComponent : public ash::Entity::ComponentBase {
public:
	engine::Model& model;
	std::vector<MeshBuffers> model_buffers;
	graph::ConstantBuffer transform_buffer;

	ModelComponent(engine::Model& model)
		: model(model)
	{}
};