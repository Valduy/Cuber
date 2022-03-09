#pragma once

#include <vector>
#include <SimpleMath.h>

#include "../ECS/IComponent.h"

class ShapeComponent : public ecs::IComponent {
public:
	std::vector<DirectX::SimpleMath::Vector3> points;
	std::vector<int> indexes;
};