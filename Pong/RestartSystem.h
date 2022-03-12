#pragma once

#include "RestartComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class RestartSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		auto it = GetIterator<RestartComponent, VelocityComponent, SpeedComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			RestartComponent& restart_system = entity.Get<RestartComponent>();
			VelocityComponent& velocity_component = entity.Get<VelocityComponent>();
			SpeedComponent& speed_component = entity.Get<SpeedComponent>();
			restart_system.delay -= dt;

			if (restart_system.delay <= 0) {
				velocity_component.x = restart_system.x;
				velocity_component.y = restart_system.y;
				speed_component.speed = 1.0f;
				entity.Remove<RestartComponent>();
			}
		}
	}
};