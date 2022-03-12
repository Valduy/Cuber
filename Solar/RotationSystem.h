#pragma once

#include "../Engine/Game.h"
#include "TransformComponent.h"
#include "RotationComponent.h"

class RotationSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		auto it = GetIterator<TransformComponent, RotationComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			TransformComponent& transform_component = entity.Get<TransformComponent>();
			RotationComponent& rotation_component = entity.Get<RotationComponent>();

			const float amount = rotation_component.speed * dt;
			const DirectX::SimpleMath::Vector3 new_rotation =
				transform_component.GetLocalRotation() + rotation_component.axis * amount;
			transform_component.SetLocalRotation(new_rotation);
		}
	}
};