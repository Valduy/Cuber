#pragma once

#include "DirectionLightComponent.h"
#include "DirectionLightData.h"
#include "../Engine/Game.h"

class UpdateLightSystem : public engine::Game::SystemBase {
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

			direction_light_component.light_data_buffer.Init(&GetRenderer(), sizeof(DirectionLightData));
			SetLookAtMatrix(transform_component, &direction_light_component);
			SetProjectionMatrix(&direction_light_component);
		}
	}

	void Update(float dt) override {
		auto camera_it = GetIterator<engine::CameraComponent>();
		if (!camera_it.HasCurrent()) return;

		auto& camera = camera_it.Get();
		auto& camera_component = camera.Get<engine::CameraComponent>();

		for (auto it = GetIterator<DirectionLightComponent>(); it.HasCurrent(); it.Next()) {
			auto& light = it.Get();
			auto& direction_light_component = light.Get<DirectionLightComponent>();
			DirectionLightData light_data{};
			light_data.view_position = camera_component.position;
			light_data.light_direction = direction_light_component.light_direction;
			light_data.light_color = direction_light_component.light_color;
			direction_light_component.light_data_buffer.Update(&light_data);
		}
	}

private:
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
