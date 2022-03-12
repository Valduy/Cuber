#pragma once

#include "CameraComponent.h"
#include "../Engine/Game.h"

class CameraSystem : public engine::Game::SystemBase {
public:
	float velocity_magnitude;
	float mouse_sensitivity;	

	CameraSystem()
		: velocity_magnitude(5.0f)
		, mouse_sensitivity(1.0f)
		, first_move_(true)
		, yaw_(0.0f)
		, pitch_(0.0f)
		, last_position_(0.0f, 0.0f)
		, direction_(0.0f, 0.0f, 0.0f)
	{}

	void Update(float dt) override {
		For<CameraComponent>([&](ecs::Entity& e) {
			CameraComponent& camera_component = e.Get<CameraComponent>();
			UpdateKeyboard();
			UpdateMouse();
			SetLookAtMatrix(&camera_component, dt);
			SetProjectionMatrix(&camera_component);
		});
	}

private:
	bool first_move_;
	float yaw_;
	float pitch_;
	DirectX::SimpleMath::Vector2 last_position_;
	DirectX::SimpleMath::Vector3 direction_;

	void UpdateKeyboard() {
		using namespace DirectX::SimpleMath;
		direction_ = Vector3::Zero;

		if (GetKeyboardState().IsKeyDown(graph::Keys::kW)) {
			direction_ += Vector3(1.0f, 0.0f, 0.0f);
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kS)) {
			direction_ += Vector3(-1.0f, 0.0f, 0.0f);
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kA)) {
			direction_ += Vector3(0.0f, 0.0f, -1.0f);
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kD)) {
			direction_ += Vector3(0.0f, 0.0f, 1.0f);
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kSpace)) {
			direction_ += Vector3(0.0f, 1.0f, 0.0f);
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kShift)) {
			direction_ += Vector3(0.0f, -1.0f, 0.0f);
		}

		direction_.Normalize();
	}

	void UpdateMouse() {
		using namespace DirectX::SimpleMath;	

		if (first_move_) {
			last_position_.x = GetMouseState().GetX();
			last_position_.y = GetMouseState().GetY();
			first_move_ = false;
		}
		else {
			static constexpr float weight = 0.03f;
			float dx = GetMouseState().GetX() - last_position_.x;
			float dy = GetMouseState().GetY() - last_position_.y;
			last_position_.x = GetMouseState().GetX();
			last_position_.y = GetMouseState().GetY();
			yaw_ -= weight * dx * mouse_sensitivity;
			pitch_ -= weight * dy * mouse_sensitivity;
			pitch_ = pitch_ < -DirectX::XM_PIDIV2
				? -DirectX::XM_PIDIV2
				: DirectX::XM_PIDIV2 < pitch_ ? DirectX::XM_PIDIV2 : pitch_;
		}
	}

	void SetLookAtMatrix(CameraComponent* camera_component, float dt) {
		using namespace DirectX::SimpleMath;
		Matrix rotation_matrix = Matrix::CreateFromYawPitchRoll(yaw_, pitch_, 0.0f);

		direction_ = rotation_matrix.Forward() * direction_.x +
			Vector3::Up * direction_.y +
			rotation_matrix.Right() * direction_.z;

		if (direction_.Length() != 0) {
			direction_.Normalize();
		}

		camera_component->position += direction_ * velocity_magnitude * dt;
		camera_component->view_matrix = Matrix::CreateLookAt(
			camera_component->position,
			camera_component->position + rotation_matrix.Forward(),
			rotation_matrix.Up());
	}

	void SetProjectionMatrix(CameraComponent* camera_component) {
		using namespace DirectX::SimpleMath;

		camera_component->projection_matrix = Matrix::CreatePerspectiveFieldOfView(
			DirectX::XM_PIDIV2,
			GetWindow().GetAspectRatio(),
			0.1f,
			100.0f);

		//camera_component->projection_matrix = Matrix::CreateOrthographic(
		//	2.0,
		//	2.0,
		//	0.1f,
		//	100.0f);
	}
}; 