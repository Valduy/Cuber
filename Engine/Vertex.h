#pragma once

#include <SimpleMath.h>

namespace engine {

class Vertex {
public:
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::SimpleMath::Vector2 texture_coords;
};

} // namespace engine