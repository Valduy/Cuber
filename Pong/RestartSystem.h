#pragma once

#include "RestartComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class RestartSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		GetGame().GetEntityManager().For<
			RestartComponent,
			VelocityComponent,
			SpeedComponent>([&](ecs::Entity& e) {
				RestartComponent& restart_system = e.Get<RestartComponent>();
				VelocityComponent& velocity_component = e.Get<VelocityComponent>();
				SpeedComponent& speed_component = e.Get<SpeedComponent>();
				restart_system.delay -= dt;

				if (restart_system.delay <= 0) {
					velocity_component.x = restart_system.x;
					velocity_component.y = restart_system.y;
					speed_component.speed = 1.0f;
					e.Remove<RestartComponent>();
				}
			});
	}
};