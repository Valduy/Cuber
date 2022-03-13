#pragma once

#include <vector>
#include <SimpleMath.h>

#include "../ECS/IComponent.h"

class LinesComponent : public ecs::IComponent {
public:
	DirectX::SimpleMath::Vector4 color { 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<DirectX::SimpleMath::Vector3> points;
};