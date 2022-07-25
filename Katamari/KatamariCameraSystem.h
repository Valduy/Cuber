#pragma once

#include "KatamariControllerComponent.h"
#include "../Engine/Game.h"
#include "../Engine/CameraSystem.h"
#include "../Engine/TransformComponent.h"

// Entity with camera component should be attached to Katamari entity.
class KatamariCameraSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		using namespace engine;

		const auto camera_it = Filter<TransformComponent, CameraComponent>().GetIterator();
		if (!camera_it.HasCurrent()) return;

		auto& [camera, camera_transform_component, camera_component] = camera_it.Get();

		const auto katamari_it = Filter<TransformComponent, KatamariControllerComponent>().GetIterator();
		if (!katamari_it.HasCurrent()) return;

		auto& [katamari, katamari_transform_component, katamari_controller_component] = katamari_it.Get();

		SetLookAtMatrix(
			&camera_component,
			camera_transform_component.GetPosition(),
			katamari_transform_component.GetPosition());
		SetProjectionMatrix(&camera_component);
	}

private:
	static void SetLookAtMatrix(
		engine::CameraComponent* camera_component, 
		DirectX::SimpleMath::Vector3 from, 
		DirectX::SimpleMath::Vector3 to)
	{
		camera_component->view_matrix = DirectX::SimpleMath::Matrix::CreateLookAt(
			from, to, DirectX::SimpleMath::Vector3::UnitY);
	}

	void SetProjectionMatrix(engine::CameraComponent* camera_component) const {
		using namespace DirectX::SimpleMath;

		camera_component->projection_matrix = Matrix::CreatePerspectiveFieldOfView(
			DirectX::XM_PIDIV2,
			GetWindow().GetAspectRatio(),
			0.1f,
			100.0f);
	}
};
