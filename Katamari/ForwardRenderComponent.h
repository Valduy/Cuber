#pragma once

#include "../ECS/Entity.h"
#include "../GraphicEngine/ConstantBuffer.h"

class ForwardRenderComponent : public ecs::Entity::ComponentBase {
public:
	graph::ConstantBuffer light_transform_buffer;
};