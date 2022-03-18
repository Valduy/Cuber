#pragma once

#include <cmath>

#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "RotationComponent.h"

class SinSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		time += dt;

		using namespace engine;
		auto it = GetIterator<TransformComponent, RotationComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& planet = it.Get();
			TransformComponent& transform = planet.Get<TransformComponent>();
			auto position = transform.GetLocalPosition();	
			position.y = 2 * sin(time);
			transform.SetLocalPosition(position);
		}
	}

private:
	float time = 0;
};
