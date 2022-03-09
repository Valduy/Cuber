#pragma once

#include "../ECS/IComponent.h"
#include <vector>

class ShapeComponent : public ecs::IComponent {
public:
	std::vector<DirectX::SimpleMath::Vector4> points;
	std::vector<int> indexes;
	DirectX::SimpleMath::Vector4 color;
};
