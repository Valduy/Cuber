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

		auto it = GetIterator<AmbientLightComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& light = it.Get();
			auto& ambient_light_component = light.Get<AmbientLightComponent>();

			ambient_light_component.light_data_buffer.Init(&GetRenderer(), sizeof(AmbientLightData));
		}
	}

	void InitDirectionLights() {
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

	void InitPointLights() {
		using namespace engine;

		for (auto it = GetIterator<PointLightComponent>(); it.HasCurrent(); it.Next()) {
			auto& light = it.Get();
			auto& point_light_component = light.Get<PointLightComponent>();

			point_light_component.light_data_buffer.Init(&GetRenderer(), sizeof(PointLightData));
		}
	}

	void UpdateAmbientLights() {
		for (auto it = GetIterator<AmbientLightComponent>(); it.HasCurrent(); it.Next()) {
			auto& light = it.Get();
			auto& ambient_light_component = light.Get<AmbientLightComponent>();

			AmbientLightData light_data{};
			light_data.light_color = ambient_light_component.light_color;
			light_data.intensity = ambient_light_component.intensity;
			ambient_light_component.light_data_buffer.Update(&light_data);
		}
	}

	void UpdateDirectionLights(DirectX::SimpleMath::Vector3 view_position, float dt) {
		for (auto it = GetIterator<DirectionLightComponent>(); it.HasCurrent(); it.Next()) {
			auto& light = it.Get();
			auto& direction_light_component = light.Get<DirectionLightComponent>();

			//auto rot = DirectX::SimpleMath::Matrix::CreateRotationY(dt);
			//direction_light_component.light_direction = DirectX::SimpleMath::Vector3::Transform(direction_light_component.light_direction, rot);
			//direction_light_component.light_direction.Normalize();

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

		auto it = GetIterator<TransformComponent, PointLightComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& light = it.Get();
			auto& transform_component = light.Get<TransformComponent>();
			auto& point_light_component = light.Get<PointLightComponent>();

			//transform_component.SetPosition(transform_component.GetPosition() + dir * dt);
			//if (transform_component.GetPosition().y < -5) {
			//	dir = -dir;
			//}
			//if (transform_component.GetPosition().y > 8) {
			//	dir = -dir;
			//}

			PointLightData light_data{};
			light_data.view_position = view_position;
			light_data.light_position = transform_component.GetPosition();
			light_data.light_color = point_light_component.light_color;
			light_data.fade_radius = point_light_component.fade_radius;
			point_light_component.light_data_buffer.Update(&light_data);
		}
	}

	bool TryGetViewPosition(DirectX::SimpleMath::Vector3* view_position) {
		auto camera_it = GetIterator<engine::CameraComponent>();
		if (!camera_it.HasCurrent()) return false;

		auto& camera = camera_it.Get();
		const auto& camera_component = camera.Get<engine::CameraComponent>();
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
