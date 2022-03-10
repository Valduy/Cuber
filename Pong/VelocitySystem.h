#pragma once

#include "SpeedComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class VelocitySystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		For<SpeedComponent, VelocityComponent, TransformComponent>(
			[&](ecs::Entity& e) {
				SpeedComponent& speed_component = e.Get<SpeedComponent>();
				VelocityComponent& velocity_component = e.Get<VelocityComponent>();
				TransformComponent& transform_component = e.Get<TransformComponent>();

				transform_component.x += speed_component.speed * velocity_component.x * dt;
				transform_component.y += speed_component.speed * velocity_component.y * dt;
			});
	}
};