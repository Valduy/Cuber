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

		auto it = GetIterator<TransformComponent, KatamariControllerComponent>();
		if (!it.HasCurrent()) return;
		ecs::Entity& katamari = it.Get();

		TransformComponent& transform_component = katamari.Get<TransformComponent>();
		KatamariControllerComponent& katamari_component = katamari.Get<KatamariControllerComponent>();
		UpdatePosition(&transform_component, &katamari_component, dt);
		UpdateRotation(&transform_component);
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
		Vector3 direction = Vector3::Zero;
		Vector3 new_angles = Vector3::Zero;

		if (GetKeyboardState().IsKeyDown(graph::Keys::kW)) {
			direction -= model.Forward();
			new_angles.x += 1;
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kS)) {
			direction += model.Forward();
			new_angles.x += 1;
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kA)) {
			direction += model.Right();
			new_angles.x += 1;
		}
		if (GetKeyboardState().IsKeyDown(graph::Keys::kD)) {
			direction -= model.Right();
			new_angles.x += 1;
		}
		
		direction.Normalize();
		const Vector3 velocity = direction * katamari_component->speed;
		engine::TransformComponent& katamari_transform = katamari_component->body.Get<engine::TransformComponent>();
		new_angles *= katamari_component->speed;
		new_angles += katamari_transform.GetLocalEuler();
		katamari_component->body.Get<engine::TransformComponent>().SetLocalEuler(new_angles);
		transform->SetPosition(transform->GetPosition() + velocity * dt);
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