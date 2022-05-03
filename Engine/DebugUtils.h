#pragma once

#include "../ECS/Entity.h"
#include "Game.h"
#include "LinesComponent.h"
#include "TransformComponent.h"

namespace engine {

class DebugUtils {
public:
	static ecs::Entity& CreatePlane(Game& game, float size, int subdivisions) {
		ecs::Entity& plane = game.GetEntityManager().CreateEntity();
		plane.Add<TransformComponent>();
		LinesComponent& lines_component = plane.Add<LinesComponent>();
		lines_component.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

		const float cell_size = size / static_cast<float>(subdivisions);
		const float pivot_x = -size / 2;
		const float pivot_z = -size / 2;

		using namespace DirectX::SimpleMath;
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

	static ecs::Entity& CreateLine(
		Game& game,
		DirectX::SimpleMath::Vector3 a,
		DirectX::SimpleMath::Vector3 b,
		DirectX::SimpleMath::Vector4 color)
	{
		using namespace DirectX::SimpleMath;
		ecs::Entity& line = game.GetEntityManager().CreateEntity();
		line.Add<TransformComponent>();
		LinesComponent& lines_component = line.Add<LinesComponent>();
		lines_component.color = color;
		lines_component.points = { a, b };
		lines_component.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		return line;
	}

	static ecs::Entity& CreateAxis(Game& game, float length) {
		ecs::Entity& axis = game.GetEntityManager().CreateEntity();
		TransformComponent& transform = axis.Add<TransformComponent>();

		ecs::Entity& x = CreateLine(
			game,
			{ 0.0f, 0.0f, 0.0f },
			{ length, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f, 1.0f });
		transform.AddChild(x);

		ecs::Entity& y = CreateLine(
			game,
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, length, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f });
		transform.AddChild(y);

		ecs::Entity& z = CreateLine(
			game,
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, length },
			{ 0.0f, 0.0f, 1.0f, 1.0f });
		transform.AddChild(z);

		return axis;
	}

	static ecs::Entity& CreateCircle(
		Game& game, 
		float radius, 
		DirectX::SimpleMath::Vector4 color)
	{
		ecs::Entity& circle = game.GetEntityManager().CreateEntity();
		circle.Add<TransformComponent>();
		LinesComponent& lines_component = circle.Add<LinesComponent>();
		lines_component.points = GetCircleDots(radius);
		lines_component.color = color;
		lines_component.topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		return circle;
	}

	static ecs::Entity& CreateSphere(
		Game& game,
		float radius,
		DirectX::SimpleMath::Vector4 color)
	{
		ecs::Entity& sphere = game.GetEntityManager().CreateEntity();
		TransformComponent& sphere_transform = sphere.Add<TransformComponent>();

		ecs::Entity& yaw = CreateCircle(game, radius, color);
		sphere_transform.AddChild(yaw);

		ecs::Entity& pitch = CreateCircle(game, radius, color);
		TransformComponent& pitch_transform = pitch.Get<TransformComponent>();
		pitch_transform.SetEuler({ 0.0f, 90.0f, 0.0f });
		sphere_transform.AddChild(pitch);

		ecs::Entity& roll = CreateCircle(game, radius, color);
		TransformComponent& roll_transform = roll.Get<TransformComponent>();
		roll_transform.SetEuler({ 90.0f, 0.0f, 0.0f });
		sphere_transform.AddChild(roll);

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