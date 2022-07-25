#pragma once

#include "KatamariControllerComponent.h"
#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"

class KatamariControllerSystem : public engine::Game::SystemBase {
public:
	float mouse_sensitivity;

	KatamariControllerSystem()
		: mouse_sensitivity(1.0f)
		, first_move_(true)
		, position_x_(0)
	{}

	void Update(float dt) override {
		using namespace engine;

		for (auto& node : Filter<TransformComponent, KatamariControllerComponent>()) {
			auto& [entity, transform_component, katamari_controller_component] = node;
			UpdatePosition(&transform_component, &katamari_controller_component, dt);
			UpdateRotation(&transform_component);
		}
	}

private:
	bool first_move_;
	int position_x_;

	void UpdatePosition(
		engine::TransformComponent* transform, 
		KatamariControllerComponent* katamari_component, 
		float dt)
	{
		using namespace DirectX::SimpleMath;
		const Matrix model = transform->GetModelMatrix();
		Vector3 input_axis = Vector3::Zero;
		Vector3 direction = Vector3::Zero;

		if (GetKeyboardState().IsKeyDown(graph::Keys::kW)) {
			direction -= model.Forward();
			input_axis.x += 1;
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kS)) {
			direction += model.Forward();
			input_axis.x -= 1;
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kA)) {
			direction += model.Right();
			input_axis.z -= 1;
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kD)) {
			direction -= model.Right();
			input_axis.z += 1;
		}
		
		direction.Normalize();
		const Vector3 velocity = direction * katamari_component->speed;
		const Vector3 movement = velocity * dt;		
		transform->SetPosition(transform->GetPosition() + movement);

		using namespace engine;
		input_axis.Normalize();
		input_axis = input_axis.Cross(Vector3::UnitY) * dt * katamari_component->angle_speed;
		const Quaternion quat = Quaternion::CreateFromYawPitchRoll(0, input_axis.z, -input_axis.x);

		auto& katamari_transform = katamari_component->body.Get<TransformComponent>();
		katamari_transform.SetLocalRotation(katamari_transform.GetLocalRotation() * quat);
	}

	void UpdateRotation(engine::TransformComponent* transform) {
		using namespace DirectX::SimpleMath;

		if (first_move_) {
			position_x_ = GetMouseState().GetX();
			first_move_ = false;
		}
		else {
			const int dy = GetMouseState().GetX() - position_x_;
			const float angle = static_cast<float>(dy) * mouse_sensitivity;
			transform->SetEuler(transform->GetEuler() -= Vector3::UnitY * angle);
			position_x_ = GetMouseState().GetX();
		}
	}
};