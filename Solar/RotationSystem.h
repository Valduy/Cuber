#pragma once

#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "RotationComponent.h"

class RotationSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		using namespace engine;
		using namespace DirectX::SimpleMath;

		for (auto& [entity, transform_component, rotation_component] : Filter<TransformComponent, RotationComponent>()) {
			const float amount = rotation_component.speed * dt;
			const Quaternion quat = Quaternion::CreateFromYawPitchRoll(rotation_component.axis * amount);
			transform_component.SetLocalRotation(transform_component.GetLocalRotation() * quat);
		}
	}
};