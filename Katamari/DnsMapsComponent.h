#pragma once

#include <DirectXTex.h>
#include "../ECS/Entity.h"
#include "../GraphicEngine/Texture.h"

class DnsMapsComponent : public ecs::Entity::ComponentBase {
public:
	DirectX::ScratchImage& diffuse;
	DirectX::ScratchImage& normal;
	DirectX::ScratchImage& specular;

	graph::Texture diffuse_texture;
	graph::Texture normal_texture;
	graph::Texture specular_texture;

	DnsMapsComponent(
		DirectX::ScratchImage& diffuse,
		DirectX::ScratchImage& normal,
		DirectX::ScratchImage& specular)
		: diffuse(diffuse)
		, normal(normal)
		, specular(specular)
	{}
};