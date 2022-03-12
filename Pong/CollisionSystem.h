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
		static const auto ball_sign = 
			ecs::Signer::GetSignature<TransformComponent, BoundingBoxComponent, SpeedComponent, VelocityComponent>();
		static const auto paddle_sign = 
			ecs::Signer::GetSignature<TransformComponent, BoundingBoxComponent, InputComponent>();

		for (auto ball_it = GetIterator(ball_sign); ball_it.HasCurrent(); ball_it.Next()) {
			ecs::Entity& ball = ball_it.Get();
			VelocityComponent& ball_velocity_component = ball.Get<VelocityComponent>();
			TransformComponent& ball_transform_component = ball.Get<TransformComponent>();
			BoundingBoxComponent& ball_bb_component = ball.Get<BoundingBoxComponent>();
			SpeedComponent& speed = ball.Get<SpeedComponent>();

			for (auto paddle_it = GetIterator(paddle_sign); paddle_it.HasCurrent(); paddle_it.Next()) {
				ecs::Entity& paddle = paddle_it.Get();
				TransformComponent& paddle_transform_component = paddle.Get<TransformComponent>();
				BoundingBoxComponent& paddle_bb_component = paddle.Get<BoundingBoxComponent>();

				if (IsBoundingBoxesIntersected(
					ball_transform_component,
					ball_bb_component,
					paddle_transform_component,
					paddle_bb_component))
				{
					float x = ball_transform_component.x - paddle_transform_component.x;
					float y = ball_transform_component.y - paddle_transform_component.y;
					float magnitude = sqrt(x * x + y * y);
					ball_velocity_component.x = x / magnitude;
					ball_velocity_component.y = y / magnitude;
					speed.speed *= 1.1f;
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