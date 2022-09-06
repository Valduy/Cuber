#pragma once

#include <SimpleMath.h>
#include "Game.h"
#include "LinesComponent.h"
#include "TransformComponent.h"

namespace engine {

class DebugUtils {
public:
	static ash::Entity& CreatePlane(Game& game, float size, int subdivisions) {
		using namespace DirectX::SimpleMath;

		ash::Entity& plane = game.GetEntities().Create();
		plane.Add<TransformComponent>();
		LinesComponent& lines_component = plane.Add<LinesComponent>();
		lines_component.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

		const float cell_size = size / static_cast<float>(subdivisions);
		const float pivot_x = -size / 2;
		const float pivot_z = -size / 2;

		
		for (int i = 0; i <= subdivisions; ++i) {
			const float x = pivot_x + cell_size * static_cast<float>(i);
			lines_component.points.push_back(Vector3(x, 0.0f, pivot_z));
			lines_component.points.push_back(Vector3(x, 0.0f, -pivot_z));

			const float z = pivot_z + cell_size * static_cast<float>(i);
			lines_component.points.push_back(Vector3(pivot_x, 0.0f, z));
			lines_component.points.push_back(Vector3(-pivot_x, 0.0f, z));
		}
		
		return plane;
	}

	static ash::Entity& CreateLine(
		Game& game,
		DirectX::SimpleMath::Vector3 a,
		DirectX::SimpleMath::Vector3 b,
		DirectX::SimpleMath::Vector4 color)
	{
		using namespace DirectX::SimpleMath;

		ash::Entity& line = game.GetEntities().Create();
		line.Add<TransformComponent>();
		LinesComponent& lines_component = line.Add<LinesComponent>();
		lines_component.color = color;
		lines_component.points = { a, b };
		lines_component.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		return line;
	}

	static ash::Entity& CreateAxis(Game& game, float length) {
		ash::Entity& axis = game.GetEntities().Create();
		axis.Add<TransformComponent>();

		ash::Entity& x = CreateLine(
			game,
			{ 0.0f, 0.0f, 0.0f },
			{ length, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f, 1.0f });
		x.Get<TransformComponent>().SetParent(axis);

		ash::Entity& y = CreateLine(
			game,
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, length, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f });
		y.Get<TransformComponent>().SetParent(axis);

		ash::Entity& z = CreateLine(
			game,
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, length },
			{ 0.0f, 0.0f, 1.0f, 1.0f });
		z.Get<TransformComponent>().SetParent(axis);

		return axis;
	}

	static ash::Entity& CreateCircle(
		Game& game, 
		float radius, 
		DirectX::SimpleMath::Vector4 color)
	{
		ash::Entity& circle = game.GetEntities().Create();
		circle.Add<TransformComponent>();
		LinesComponent& lines_component = circle.Add<LinesComponent>();
		lines_component.points = GetCircleDots(radius);
		lines_component.color = color;
		lines_component.topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		return circle;
	}

	static ash::Entity& CreateSphere(
		Game& game,
		float radius,
		DirectX::SimpleMath::Vector4 color)
	{
		ash::Entity& sphere = game.GetEntities().Create();
		sphere.Add<TransformComponent>();

		ash::Entity& yaw = CreateCircle(game, radius, color);
		auto& yaw_transform = yaw.Get<TransformComponent>();
		yaw_transform.SetParent(sphere);

		auto& pitch = CreateCircle(game, radius, color);
		auto& pitch_transform = pitch.Get<TransformComponent>();
		pitch_transform.SetEuler({ 0.0f, 90.0f, 0.0f });
		pitch_transform.SetParent(sphere);

		auto& roll = CreateCircle(game, radius, color);
		auto& roll_transform = roll.Get<TransformComponent>();
		roll_transform.SetEuler({ 90.0f, 0.0f, 0.0f });
		roll_transform.SetParent(sphere);

		return sphere;
	}

private:
	static std::vector<DirectX::SimpleMath::Vector3> GetCircleDots(float radius) {
		std::vector<DirectX::SimpleMath::Vector3> dots;

		for (int angle = 0; angle <= 360; angle += 10) {
			float dot_x = cos(angle * DirectX::XM_PI / 180) * radius;
			float dot_y = sin(angle * DirectX::XM_PI / 180) * radius;
			dots.push_back({ dot_x, dot_y, 0.0f });
		}

		return dots;
	}
};

} // namespace engine