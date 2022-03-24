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

		auto camera_it = GetIterator<TransformComponent, CameraComponent>();
		if (!camera_it.HasCurrent()) return;
		ecs::Entity& camera = camera_it.Get();

		auto katamari_it = GetIterator<TransformComponent, KatamariControllerComponent>();
		if (!katamari_it.HasCurrent()) return;
		ecs::Entity& katamari = katamari_it.Get();
		
		CameraComponent& camera_component = camera.Get<CameraComponent>();
		const TransformComponent& camera_transform = camera.Get<TransformComponent>();
		const TransformComponent& karamari_transform = katamari.Get<TransformComponent>();
		camera_component.position = camera_transform.GetPosition();

		SetLookAtMatrix(
			&camera_component,
			camera_transform.GetPosition(),
			karamari_transform.GetPosition());
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

	void SetProjectionMatrix(engine::CameraComponent* camera_component) {
		using namespace DirectX::SimpleMath;

		camera_component->projection_matrix = Matrix::CreatePerspectiveFieldOfView(
			DirectX::XM_PIDIV2,
			GetWindow().GetAspectRatio(),
			0.1f,
			100.0f);
	}
};
