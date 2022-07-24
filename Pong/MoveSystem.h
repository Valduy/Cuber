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
		for (auto& node : Filter<TransformComponent, SpeedComponent, InputComponent, BoundingBoxComponent>()) {
			auto& [entity, transform_component, speed_component, input_component, bounding_box_component] = node;

			if (input_component.up) {
				if (transform_component.y + bounding_box_component.height / 2 < 2.0f) {
					transform_component.y += speed_component.speed * dt;
				}
			}
			if (input_component.down) {
				if (transform_component.y - bounding_box_component.height / 2 > 0.0f) {
					transform_component.y -= speed_component.speed * dt;
				}
			}
		}
	}
};