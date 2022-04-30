#pragma once

#include <SimpleMath.h>

struct ModelTransformData {
	DirectX::SimpleMath::Matrix world;
	DirectX::SimpleMath::Matrix world_view_proj;
	DirectX::SimpleMath::Matrix inverse_transpose_world;
};
