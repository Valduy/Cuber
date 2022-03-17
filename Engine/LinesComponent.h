#pragma once

#include <vector>
#include <SimpleMath.h>

#include "../ECS/Entity.h"

class LinesComponent : public ecs::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector4 color { 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<DirectX::SimpleMath::Vector3> points;
};