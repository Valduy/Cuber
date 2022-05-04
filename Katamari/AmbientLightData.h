#pragma once

#include <SimpleMath.h>

struct AmbientLightData {
	DirectX::SimpleMath::Vector3 light_color;
	float dummy0;
	float intensity;
	float dummy1;
	float dummy2;
	float dummy3;
};
