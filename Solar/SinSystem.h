#pragma once

#include <cmath>

#include "RotationComponent.h"
#include "TransformComponent.h"
#include "../Engine/Game.h"

class SinSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		time += dt;
		auto it = GetIterator<TransformComponent, RotationComponent>();

		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& planet = it.Get();
			TransformComponent& transform = planet.Get<TransformComponent>();
			auto position = transform.GetLocalPosition();	
			auto y = 2 * sin(time);
			position.y = y;
			transform.SetLocalPosition(position);
		}
	}

private:
	float time = 0;
};
