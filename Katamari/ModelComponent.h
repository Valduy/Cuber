#pragma once

#include "../ECS/Entity.h"
#include "../Engine/Model.h"

class ModelComponent : public ecs::Entity::ComponentBase {
public:
	engine::Model model;
	DirectX::ScratchImage texture;

	~ModelComponent() override {
		texture.Release();
	}

	void Delete() override {
		texture.Release();
	}
};