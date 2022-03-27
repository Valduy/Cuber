#pragma once

#include "../ECS/Entity.h"
#include "../Engine/Model.h"

class ModelComponent : public ecs::Entity::ComponentBase {
public:
	engine::Model& model;
	DirectX::ScratchImage& texture;

	ModelComponent(
		engine::Model& model,
		DirectX::ScratchImage& texture)
		: model(model)
		, texture(texture)
	{}
};