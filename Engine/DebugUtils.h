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
};

} // namespace engine