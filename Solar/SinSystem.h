#pragma once

#include <cmath>

#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "RotationComponent.h"

class SinSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		using namespace engine;

		time += dt;

		for (auto& [entity, transform_component, rotation_component] : Filter<TransformComponent, RotationComponent>()) {
			auto position = transform_component.GetLocalPosition();	
			position.y = 2 * sin(time);
			transform_component.SetLocalPosition(position);
		}
	}

private:
	float time = 0;
};
