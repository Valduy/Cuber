#pragma once

#include <vector>
#include <SimpleMath.h>

#include "../ECS/ComponentBase.h"

class ShapeComponent : public ecs::ComponentBase {
public:
	std::vector<DirectX::SimpleMath::Vector3> points;
	std::vector<int> indexes;
};