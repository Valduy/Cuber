#pragma once

#include <SimpleMath.h>

#include "../ECS/IComponent.h"

class CameraComponent : public ecs::IComponent {
public:
	DirectX::SimpleMath::Matrix view_matrix;
	DirectX::SimpleMath::Matrix projection_matrix;
	
	DirectX::SimpleMath::Matrix GetCameraMatrix() const {
		return view_matrix * projection_matrix;
	}

	DirectX::SimpleMath::Vector3 GetPosition() const {
		DirectX::SimpleMath::Matrix inv;
		view_matrix.Invert(inv);
		return inv.Translation();
	}

	DirectX::SimpleMath::Vector3 GetForwardVector() const {
		DirectX::SimpleMath::Matrix inv;
		view_matrix.Invert(inv);
		return inv.Forward();
	}

	DirectX::SimpleMath::Vector3 GetUpVector() const {
		DirectX::SimpleMath::Matrix inv;
		view_matrix.Invert(inv);
		return inv.Up();
	}
};