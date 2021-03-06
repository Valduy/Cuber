#pragma once

#include <SimpleMath.h>

#include "../ECS/Entity.h"
#include <vector>

class ShapeComponent : public ecs::Entity::ComponentBase {
public:
	std::vector<DirectX::SimpleMath::Vector4> points;
	std::vector<int> indexes;
	DirectX::SimpleMath::Vector4 color;
};
