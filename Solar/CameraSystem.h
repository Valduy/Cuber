#pragma once

#include "CameraComponent.h"
#include "../Engine/Game.h"

class CameraSystem : public engine::Game::SystemBase {
public:
	float velocity_magnitude;
	float mouse_sensitivity;
	DirectX::SimpleMath::Vector3 camera_position;

	CameraSystem()
		: velocity_magnitude(5.0f)
		, mouse_sensitivity(1.0f)
		, camera_position(0.0f, 0.0f, 0.0f)
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

			using namespace DirectX::SimpleMath;
			Matrix rotation_matrix = Matrix::CreateFromYawPitchRoll(yaw_, pitch_, 0.0f);

			direction_ = rotation_matrix.Forward() * direction_.x +
				Vector3::Up * direction_.y +
				rotation_matrix.Right() * direction_.z;

			if (direction_.Length() != 0) {
				direction_.Normalize();
			}

			camera_position += direction_ * velocity_magnitude * dt;
			camera_component.view_matrix = Matrix::CreateLookAt(
				camera_position,
				camera_position + rotation_matrix.Forward(),
				rotation_matrix.Up());

			float aspect_ratio = static_cast<float>(GetWindow().GetWidth()) / static_cast<float>(GetWindow().GetHeight());

			camera_component.projection_matrix = Matrix::CreatePerspectiveFieldOfView(
				DirectX::XM_PIDIV2, 
				aspect_ratio, 
				0.1f, 
				100.0f);
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
		if (GetKeyboardState().IsKeyDown(graph::Keys::kLShift)) {
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
}; 