#pragma once

#include <SimpleMath.h>
#include <vector>

#include "../Ash/Entity.h"

class ShapeComponent : public ash::Entity::ComponentBase {
public:
	std::vector<DirectX::SimpleMath::Vector4> points;
	std::vector<int> indexes;
	DirectX::SimpleMath::Vector4 color;
};
