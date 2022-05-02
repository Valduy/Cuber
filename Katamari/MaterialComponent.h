#pragma once

#include "Material.h"
#include "../ECS/Entity.h"
#include "../GraphicEngine/ConstantBuffer.h"

class MaterialComponent : public ecs::Entity::ComponentBase {
public:
	Material material;
	graph::ConstantBuffer material_buffer;
};