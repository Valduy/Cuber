#pragma once

#include <vector>
#include <SimpleMath.h>

#include "../ECS/Entity.h"

class ShapeComponent : public ecs::Entity::ComponentBase {
public:
	std::vector<DirectX::SimpleMath::Vector3> points;
	std::vector<int> indexes;
};