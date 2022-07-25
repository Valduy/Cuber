#pragma once

#include "AmbientLightComponent.h"
#include "AmbientLightData.h"
#include "DirectionLightComponent.h"
#include "DirectionLightData.h"
#include "PointLightComponent.h"
#include "PointLightData.h"
#include "../Engine/Game.h"

class UpdateLightSystem : public engine::Game::SystemBase {
public:
	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		InitAmbientLights();
		InitDirectionLights();
		InitPointLights();
	}

	void Update(float dt) override {
		UpdateAmbientLights();

		DirectX::SimpleMath::Vector3 view_position;
		if (!TryGetViewPosition(&view_position)) return;

		UpdateDirectionLights(view_position, dt);
		UpdatePointLights(view_position, dt);
	}

private:
	void InitAmbientLights() {
		using namespace engine;

		for (auto& [entity, ambient_light_component] : Filter<AmbientLightComponent>()) {
			ambient_light_component.light_data_buffer.Init(&GetRenderer(), sizeof(AmbientLightData));
		}
	}

	void InitDirectionLights() {
		using namespace engine;

		for (auto& [entity, transform_component, direction_light_component] : Filter<TransformComponent, DirectionLightComponent>()) {
			direction_light_component.light_data_buffer.Init(&GetRenderer(), sizeof(DirectionLightData));
			SetLookAtMatrix(transform_component, &direction_light_component);
			SetProjectionMatrix(&direction_light_component);
		}
	}

	void InitPointLights() {
		using namespace engine;

		for (auto& [entity, point_light_component] : Filter<PointLightComponent>()) {
			point_light_component.light_data_buffer.Init(&GetRenderer(), sizeof(PointLightData));
		}
	}

	void UpdateAmbientLights() {
		for (auto& [entity, ambient_light_component] : Filter<AmbientLightComponent>()) {
			AmbientLightData light_data{};
			light_data.light_color = ambient_light_component.light_color;
			light_data.intensity = ambient_light_component.intensity;
			ambient_light_component.light_data_buffer.Update(&light_data);
		}
	}

	void UpdateDirectionLights(DirectX::SimpleMath::Vector3 view_position, float dt) {
		for (auto& [entity, direction_light_component] : Filter<DirectionLightComponent>()) {
			DirectionLightData light_data{};
			light_data.view_position = view_position;
			light_data.light_direction = direction_light_component.light_direction;
			light_data.light_color = direction_light_component.light_color;
			direction_light_component.light_data_buffer.Update(&light_data);
		}
	}

	void UpdatePointLights(DirectX::SimpleMath::Vector3 view_position, float dt) {
		using namespace engine;
		static DirectX::SimpleMath::Vector3 dir = { 0.0f, 1.0f, 0.0f };

		for (auto& [entity, transform_component, point_light_component] : Filter<TransformComponent, PointLightComponent>()) {
			PointLightData light_data{};
			light_data.view_position = view_position;
			light_data.light_position = transform_component.GetPosition();
			light_data.light_color = point_light_component.light_color;
			light_data.fade_radius = point_light_component.fade_radius;
			point_light_component.light_data_buffer.Update(&light_data);
		}
	}

	bool TryGetViewPosition(DirectX::SimpleMath::Vector3* view_position) {
		auto it = Filter<engine::CameraComponent>().GetIterator();
		if (!it.HasCurrent()) return false;

		auto& [entity, camera_component] = it.Get();
		*view_position = camera_component.position;
		return true;
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
