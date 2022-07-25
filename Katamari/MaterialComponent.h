#pragma once

#include "Material.h"
#include "../Ash/Entity.h"
#include "../GraphicEngine/ConstantBuffer.h"

class MaterialComponent : public ash::Entity::ComponentBase {
public:
	Material material;
	graph::ConstantBuffer material_buffer;
};