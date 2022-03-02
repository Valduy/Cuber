#pragma once

#include "InputComponent.h"
#include "MoveSystem.h"
#include "SpeedComponent.h"
#include "TransformComponent.h"

#include "../Engine/Game.h"

class MoveSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		GetGame().GetEntityManager().For<TransformComponent, SpeedComponent, InputComponent>([&](ecs::Entity& e) {
			TransformComponent& transform_component = e.Get<TransformComponent>();
			SpeedComponent& speed_component = e.Get<SpeedComponent>();
			InputComponent& input_component = e.Get<InputComponent>();

			if (input_component.up) {
				transform_component.y += speed_component.speed * dt;
			}
			if (input_component.down) {
				transform_component.y -= speed_component.speed * dt;
			}			
		});
	}
};