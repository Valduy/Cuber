#pragma once

#include <cmath>

#include "BoundingBoxComponent.h"
#include "InputComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"
#include "../Engine/Game.h"

class CollisionSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		for (auto& ball_node : Filter<TransformComponent, BoundingBoxComponent, SpeedComponent, VelocityComponent>()) {
			auto& [	ball, 
					ball_transform_component, 
					ball_bounding_box_component, 
					ball_speed_component, 
					ball_velocity_component] = ball_node;

			for (auto& paddle_node : Filter<TransformComponent, BoundingBoxComponent, InputComponent>()) {
				auto& [	paddle, 
						paddle_transform_component, 
						paddle_bounding_box_component, 
						paddle_input_component] = paddle_node;

				if (IsBoundingBoxesIntersected(
					ball_transform_component, ball_bounding_box_component, paddle_transform_component, paddle_bounding_box_component))
				{
					const float x = ball_transform_component.x - paddle_transform_component.x;
					const float y = ball_transform_component.y - paddle_transform_component.y;
					const float magnitude = sqrt(x * x + y * y);

					ball_velocity_component.x = x / magnitude;
					ball_velocity_component.y = y / magnitude;
					ball_speed_component.speed *= 1.1f;
				}
			} 
		}
	}

private:
	bool IsBoundingBoxesIntersected(
		const TransformComponent& t1, const BoundingBoxComponent& bb1,
		const TransformComponent& t2, const BoundingBoxComponent& bb2) const
	{
		return IsBoundingBoxesIntersected(
			t1.x, t1.y, bb1.width, bb1.height,
			t2.x, t2.y, bb2.width, bb2.height);
	}

	bool IsBoundingBoxesIntersected(
		float x1, float y1, float width1, float height1,
		float x2, float y2, float width2, float height2) const
	{
		return abs(x1 - x2) * 2 < (width1 + width2)
			&& abs(y1 - y2) * 2 < (height1 + height2);
	}
};