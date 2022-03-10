#pragma once

#include "CameraComponent.h"
#include "../Engine/Game.h"

class CameraSystem : public engine::Game::SystemBase {
public:
	float velocity_magnitude;
	float mouse_sensitivity;
	
	CameraSystem()
		: yaw(0.0f)
		, pitch(0.0f)
		, velocity_magnitude(100.0f)
		, mouse_sensitivity(1.0f)
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		For<CameraComponent>([&](ecs::Entity& e) {
			CameraComponent& camera_component = e.Get<CameraComponent>();

			using namespace DirectX::SimpleMath;
			Vector3 eye(0.0f, 3.0f, -3.0f);
			Vector3 at(0.0f, 0.0f, 0.0f);
			Vector3 up(0.0f, 1.0f, 0.0f);

			camera_component.view_matrix= Matrix::CreateLookAt(eye, at, up);

			camera_component.projection_matrix = Matrix::CreatePerspectiveFieldOfView(
				DirectX::XM_PIDIV2, 1.0f, 0.1f, 100.0f);
		});
	}

	void Update(float dt) override {
		For<CameraComponent>([&](ecs::Entity e) {
			UpdateMouse();
		});
	}

private:
	DirectX::SimpleMath::Vector2 last_position;
	DirectX::SimpleMath::Vector3 direction;
	float yaw;
	float pitch;

	void UpdateKeyboard() {
		graph::KeyboardState& keyboard_state = GetGame().GetWindow().GetKeyboardState();

	}

	void UpdateMouse() {
		graph::MouseState& mouse_state = GetGame().GetWindow().GetMouseState();
		yaw -= mouse_state.GetX() * 0.003f * mouse_sensitivity;
		pitch -= mouse_state.GetY() * 0.003f * mouse_sensitivity;
	}
}; 