#pragma once

#include "BoundingBoxComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class BoundingBoxComponent;

class BorderSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		auto it = GetIterator<TransformComponent, BoundingBoxComponent, VelocityComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			TransformComponent& transform_component = entity.Get<TransformComponent>();
			BoundingBoxComponent& bounding_box_component = entity.Get<BoundingBoxComponent>();
			VelocityComponent& velocity_component = entity.Get<VelocityComponent>();

			if (transform_component.y + bounding_box_component.height / 2 >= 2.0f
				|| transform_component.y - bounding_box_component.height / 2 <= 0.0f)
			{
				velocity_component.y *= -1;
			}
		}
	}
};
