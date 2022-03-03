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
		GetGame().GetEntityManager().For<
			TransformComponent,
			SpeedComponent,
			InputComponent,
			BoundingBoxComponent>([&](ecs::Entity& e) 
			{
				TransformComponent& transform_component = e.Get<TransformComponent>();
				SpeedComponent& speed_component = e.Get<SpeedComponent>();
				InputComponent& input_component = e.Get<InputComponent>();
				BoundingBoxComponent& bounding_box_component = e.Get<BoundingBoxComponent>();

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
			});
	}
};