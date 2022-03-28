#pragma once

#include "Material.h"
#include "../ECS/Entity.h"
#include "../Engine/Model.h"

class ModelComponent : public ecs::Entity::ComponentBase {
public:
	engine::Model& model;
	DirectX::ScratchImage& texture;
	Material material;

	ModelComponent(
		engine::Model& model,
		DirectX::ScratchImage& texture,
		Material material)
		: model(model)
		, texture(texture)
		, material(material)
	{}
};