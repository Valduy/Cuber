#pragma once

#include "SpeedComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class VelocitySystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		auto it = GetIterator<SpeedComponent, VelocityComponent, TransformComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			SpeedComponent& speed_component = entity.Get<SpeedComponent>();
			VelocityComponent& velocity_component = entity.Get<VelocityComponent>();
			TransformComponent& transform_component = entity.Get<TransformComponent>();

			transform_component.x += speed_component.speed * velocity_component.x * dt;
			transform_component.y += speed_component.speed * velocity_component.y * dt;
		}
	}
};