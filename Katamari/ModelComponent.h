#pragma once

#include "../ECS/Entity.h"
#include "../Engine/Model.h"

class ModelComponent : public ecs::Entity::ComponentBase {
public:
	const engine::Model& model;

	ModelComponent(const engine::Model& model)
		: model(model)
	{}
};