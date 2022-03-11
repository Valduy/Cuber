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

			using namespace DirectX::SimpleMath;
			Vector3 new_rotation = transform_component.GetLocalRotation() + 
				rotation_component.axis * rotation_component.speed * dt;
			transform_component.SetLocalRotation(new_rotation);
			//transform_component.SetRotation(Vector3(0.0f, 45.0f, 0.0f));
		});
	}
};