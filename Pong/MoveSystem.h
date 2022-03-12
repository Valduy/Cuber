#pragma once

#include "BoundingBoxComponent.h"
#include "InputComponent.h"
#include "MoveSystem.h"
#include "SpeedComponent.h"
#include "TransformComponent.h"

#include "../Engine/Game.h"

class MoveSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		auto it = GetIterator<TransformComponent, SpeedComponent, InputComponent, BoundingBoxComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			TransformComponent& transform_component = entity.Get<TransformComponent>();
			SpeedComponent& speed_component = entity.Get<SpeedComponent>();
			InputComponent& input_component = entity.Get<InputComponent>();
			BoundingBoxComponent& bb_component = entity.Get<BoundingBoxComponent>();

			if (input_component.up) {
				if (transform_component.y + bb_component.height / 2 < 2.0f) {
					transform_component.y += speed_component.speed * dt;
				}
			}
			if (input_component.down) {
				if (transform_component.y - bb_component.height / 2 > 0.0f) {
					transform_component.y -= speed_component.speed * dt;
				}
			}
		}
	}
};