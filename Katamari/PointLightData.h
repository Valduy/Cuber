#pragma once

#include <SimpleMath.h>

struct PointLightData {
	DirectX::SimpleMath::Vector3 view_position;	
	float dummy0;
	DirectX::SimpleMath::Vector3 light_position;
	float dummy1;
	DirectX::SimpleMath::Vector3 light_color;
	float dummy2;
	float fade_radius;
	DirectX::SimpleMath::Vector3 dummy3;
};
