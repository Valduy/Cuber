#pragma once

#include <SimpleMath.h>

struct AmbientLightData {
	DirectX::SimpleMath::Vector3 light_color;
	float dummy0;
	float intensity;
	DirectX::SimpleMath::Vector3 dummy1;
};
