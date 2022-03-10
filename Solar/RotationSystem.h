#pragma once

#include "../Engine/Game.h"
#include "TransformComponent.h"
#include "RotationComponent.h"

class RotationSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		For<TransformComponent, RotationComponent>([&](ecs::Entity& e) {
			TransformComponent& transform_component = e.Get<TransformComponent>();
			RotationComponent& rotation_component = e.Get<RotationComponent>();
			transform_component.rotation.y += rotation_component.speed * dt;
		});
	}
};