#pragma once

#include "../ECS/ComponentBase.h"
#include <vector>

class ShapeComponent : public ecs::ComponentBase {
public:
	std::vector<DirectX::SimpleMath::Vector4> points;
	std::vector<int> indexes;
	DirectX::SimpleMath::Vector4 color;
};
