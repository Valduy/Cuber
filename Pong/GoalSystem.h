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
		For<TransformComponent, BoundingBoxComponent, VelocityComponent>(
			[&](ecs::Entity& e) {
				TransformComponent& transform_component = e.Get<TransformComponent>();
				BoundingBoxComponent& bounding_box_component = e.Get<BoundingBoxComponent>();

				if (transform_component.x + bounding_box_component.width / 2 <= 0.0f) {
					right_score += 1;
					Restart(e, -1, 0);
				}
				else if (transform_component.x - bounding_box_component.width / 2 >= 2.0f) {
					left_score += 1;
					Restart(e, 1, 0);
				}
			});
	}

private:
	int left_score = 0;
	int right_score = 0;

	void Restart(ecs::Entity& e, float x, float y) {
		TransformComponent& transform_component = e.Get<TransformComponent>();
		transform_component.x = 1.0f;
		transform_component.y = 1.0f;

		VelocityComponent& velocity_component = e.Get<VelocityComponent>();
		velocity_component.x = 0;
		velocity_component.y = 0;

		RestartComponent& restart_component = e.Add<RestartComponent>();
		restart_component.delay = kDelay;
		restart_component.x = x;
		restart_component.y = y;

		std::cout << left_score << "/" << right_score << std::endl;
	}
};
