#include <vector>

#include "../Engine/Game.h"
#include "../ECS/Entity.h"
#include "RenderSystem.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ShapeComponent& CreateBox(ecs::Entity& e) {
	ShapeComponent& shape_component = e.Add<ShapeComponent>();
	shape_component.points = {
		DirectX::SimpleMath::Vector3(-0.5f,  0.5f, -0.5f), // Top face
		DirectX::SimpleMath::Vector3( 0.5f,  0.5f, -0.5f),
		DirectX::SimpleMath::Vector3( 0.5f,  0.5f,  0.5f),
		DirectX::SimpleMath::Vector3(-0.5f,  0.5f,  0.5f),

		DirectX::SimpleMath::Vector3(-0.5f, -0.5f,  0.5f), // Bottom face
		DirectX::SimpleMath::Vector3( 0.5f, -0.5f,  0.5f),
		DirectX::SimpleMath::Vector3( 0.5f, -0.5f, -0.5f),
		DirectX::SimpleMath::Vector3(-0.5f, -0.5f, -0.5f),
	};
	shape_component.indexes = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		3, 2, 5,
		3, 5, 4,

		2, 1, 6,
		2, 6, 5,

		1, 7, 6,
		1, 0, 7,

		0, 3, 4,
		0, 4, 7
	};

	return shape_component;
}

int main() {
	engine::Game game;

	RenderSystem render_system(game);

	game.PushSystem(render_system);

	ecs::Entity& cube = game.GetEntityManager().AddEntity();
	CreateBox(cube);
	TransformComponent& tc = cube.Add<TransformComponent>();
	tc.rotation.y = 0;
	tc.rotation.z = 0;

	game.Run();
	return 0;
}