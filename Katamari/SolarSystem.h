#pragma once

#include "DirectionLightComponent.h"
#include "../Engine/Game.h"

class SolarSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		using namespace DirectX::SimpleMath;

		for (auto it = GetIterator<DirectionLightComponent>(); it.HasCurrent(); it.Next()) {
			constexpr float angle_speed = 0.5;
			auto& sun = it.Get();
			auto& light = sun.Get<DirectionLightComponent>();
			Matrix rotation = Matrix::CreateFromAxisAngle(Vector3::UnitY, angle_speed * dt);
			light.light_direction = Vector3::Transform(light.light_direction, rotation);
		}
	}
};
