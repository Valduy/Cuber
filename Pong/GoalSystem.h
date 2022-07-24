#pragma once

#include "../Engine/Game.h"
#include "BoundingBoxComponent.h"
#include "RestartComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"

class GoalSystem : public engine::Game::SystemBase {
public:
	const float kDelay = 2;

	void Update(float dt) override {
		for (auto& node : Filter<TransformComponent, BoundingBoxComponent, VelocityComponent>()) {
			auto& [entity, transform_component, bounding_box_component, velocity_component] = node;

			if (transform_component.x + bounding_box_component.width / 2 <= 0.0f) {
				right_score += 1;
				Restart(entity, transform_component, velocity_component, -1, 0);
			}
			else if (transform_component.x - bounding_box_component.width / 2 >= 2.0f) {
				left_score += 1;
				Restart(entity, transform_component, velocity_component, 1, 0);
			}
		}
	}

private:
	int left_score = 0;
	int right_score = 0;

	void Restart(ash::Entity& e, TransformComponent& transform, VelocityComponent& velocity, float x, float y) {
		transform.x = 1.0f;
		transform.y = 1.0f;

		velocity.x = 0;
		velocity.y = 0;

		RestartComponent& restart_component = e.Add<RestartComponent>();
		restart_component.delay = kDelay;
		restart_component.x = x;
		restart_component.y = y;

		std::cout << left_score << "/" << right_score << std::endl;
	}
};
