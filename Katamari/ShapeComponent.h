#pragma once

#include "../ECS/Entity.h"
#include "../Engine/Model.h"

class ShapeComponent : public ecs::Entity::ComponentBase {
public:
	const engine::Model& model;

	ShapeComponent(const engine::Model& model)
		: model(model)
	{}
};