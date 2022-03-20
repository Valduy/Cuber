#pragma once

#include "../ECS/Entity.h"
#include "../Engine/Model.h"

class ModelComponent : public ecs::Entity::ComponentBase {
public:
	const engine::Model& model;
	const DirectX::ScratchImage& texture;

	ModelComponent(
		const engine::Model& model,
		const DirectX::ScratchImage& texture)
		: model(model)
		, texture(texture)
	{}
};