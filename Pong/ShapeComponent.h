#pragma once

#include "../ECS/IComponent.h"
#include <directxmath.h>
#include <vector>

class ShapeComponent : public ecs::IComponent {
public:
	std::vector<DirectX::XMFLOAT4> points;
	std::vector<int> indexes;
	DirectX::XMFLOAT4 color;
};
