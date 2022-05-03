#pragma once

#include <SimpleMath.h>

struct DirectionLightData {
	DirectX::SimpleMath::Vector3 view_position;
	float dummy0;
	DirectX::SimpleMath::Vector3 light_direction;
	float dummy1;
	DirectX::SimpleMath::Vector3 light_color;
	float dummy2;
};
