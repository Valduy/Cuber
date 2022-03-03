#pragma once

#include "BoundingBoxComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class BoundingBoxComponent;

class BorderSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		GetGame().GetEntityManager().For<
			TransformComponent,
			BoundingBoxComponent,
			VelocityComponent>([](ecs::Entity& e) {
				TransformComponent& transform_component = e.Get<TransformComponent>();
				BoundingBoxComponent& bounding_box_component = e.Get<BoundingBoxComponent>();
				VelocityComponent& velocity_component = e.Get<VelocityComponent>();

				if (transform_component.y + bounding_box_component.height / 2 >= 2.0f 
					|| transform_component.y - bounding_box_component.height / 2 <= 0.0f) {
					velocity_component.y *= -1;
				}
			});
	}
};
