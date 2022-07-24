#pragma once

#include "RestartComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class RestartSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		for (auto& node : Filter<RestartComponent, VelocityComponent, SpeedComponent>()) {
			auto& [entity, restart_component, velocity_component, speed_component] = node;
			restart_component.delay -= dt;

			if (restart_component.delay <= 0) {
				velocity_component.x = restart_component.x;
				velocity_component.y = restart_component.y;
				speed_component.speed = 1.0f;
				entity.Remove<RestartComponent>();
			}
		}
	}
};