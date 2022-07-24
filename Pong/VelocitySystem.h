#pragma once

#include "SpeedComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class VelocitySystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		for (auto& node : Filter<SpeedComponent, VelocityComponent, TransformComponent>()) {
			auto& [entity, speed_component, velocity_component, transform_component] = node;
			transform_component.x += speed_component.speed * velocity_component.x * dt;
			transform_component.y += speed_component.speed * velocity_component.y * dt;
		}
	}
};