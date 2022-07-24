#pragma once

#include "BoundingBoxComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class BoundingBoxComponent;

class BorderSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		for (auto& node : Filter<TransformComponent, BoundingBoxComponent, VelocityComponent>()) {
			auto& [entity, transform_component, bounding_box_component, velocity_component] = node;

			if (transform_component.y + bounding_box_component.height / 2 >= 2.0f
				|| transform_component.y - bounding_box_component.height / 2 <= 0.0f)
			{
				velocity_component.y *= -1;
			}
		}
	}
};
