#pragma once

#include <vector>
#include <SimpleMath.h>

#include "../Ash/Entity.h"

namespace engine {

class LinesComponent : public ash::Entity::ComponentBase {
public:
	DirectX::SimpleMath::Vector4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<DirectX::SimpleMath::Vector3> points;
	D3D11_PRIMITIVE_TOPOLOGY topology;
};

} // namespace engine
