#pragma once

#include <vector>
#include <SimpleMath.h>

#include "../ECS/IComponent.h"

class LinesComponent : public ecs::IComponent {
public:
	std::vector<DirectX::SimpleMath::Vector3> points;
};