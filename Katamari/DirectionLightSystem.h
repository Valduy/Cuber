#pragma once

#include "DirectionLightComponent.h"
#include "../Engine/Game.h"

class DirectionLightSystem : public engine::Game::SystemBase {
public:
	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		using namespace DirectX::SimpleMath;
		using namespace engine;

		auto it = GetIterator<TransformComponent, DirectionLightComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& light = it.Get();
			auto& transform_component = light.Get<TransformComponent>();
			auto& direction_light_component = light.Get<DirectionLightComponent>();

			SetLookAtMatrix(transform_component, &direction_light_component);
			SetProjectionMatrix(&direction_light_component);
		}
	}

	static void SetLookAtMatrix(
		const engine::TransformComponent& transform_component,
		DirectionLightComponent* direction_light_component)
	{
		direction_light_component->view_matrix = DirectX::SimpleMath::Matrix::CreateLookAt(
			transform_component.GetPosition(),
			transform_component.GetPosition() + direction_light_component->light_direction,
			DirectX::SimpleMath::Vector3::UnitY);
	}

	static void SetProjectionMatrix(DirectionLightComponent* direction_light_component) {
		direction_light_component->projection_matrix = 
			DirectX::SimpleMath::Matrix::CreateOrthographicOffCenter(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 30.0f);
	}
};
