#pragma once

#include "Material.h"
#include "../ECS/Entity.h"
#include "../Engine/Model.h"

class ModelComponent : public ecs::Entity::ComponentBase {
public:
	engine::Model& model;
	DirectX::ScratchImage& diffuse;
	DirectX::ScratchImage& normal;
	DirectX::ScratchImage& specular;	
	Material material;

	ModelComponent(
		engine::Model& model,
		DirectX::ScratchImage& diffuse,
		DirectX::ScratchImage& normal,
		DirectX::ScratchImage& specular,
		Material material)
		: model(model)
		, diffuse(diffuse)
		, normal(normal)
		, specular(specular)
		, material(material)
	{}
};