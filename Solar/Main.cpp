#include <vector>

#include "../Engine/Game.h"
#include "../ECS/Entity.h"
#include "RenderSystem.h"
#include "RotationSystem.h"
#include "CameraSystem.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

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

	CameraSystem camera_system;
	RenderSystem render_system(game);
	RotationSystem rotation_system;

	camera_system.camera_position.z = -3.0f;

	game.PushSystem(camera_system);
	game.PushSystem(render_system);
	game.PushSystem(rotation_system);	

	ecs::Entity& camera = game.GetEntityManager().CreateEntity();
	camera.Add<CameraComponent>();

	ecs::Entity& cube1 = game.GetEntityManager().CreateEntity();
	CreateBox(cube1);
	TransformComponent& transform1 = cube1.Add<TransformComponent>([&] {
		return new TransformComponent(cube1);
	});
	transform1.SetLocalScale(DirectX::SimpleMath::Vector3(2.0f, 2.0f, 2.0f));
	RotationComponent& rotation1 = cube1.Add<RotationComponent>();
	rotation1.axis = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	rotation1.speed = 25.0f;

	ecs::Entity& cube2 = game.GetEntityManager().CreateEntity();
	CreateBox(cube2);
	TransformComponent& transform2 = cube2.Add<TransformComponent>([&] {
		return new TransformComponent(cube2);
	});
	transform2.SetLocalPosition(DirectX::SimpleMath::Vector3(10.0f, 0.0f, 0.0f));
	transform2.SetLocalScale(DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f));
	transform1.AddChild(cube2);
	RotationComponent& rotation2 = cube2.Add<RotationComponent>();
	rotation2.axis = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	rotation2.speed = 45.0f;

	ecs::Entity& cube3 = game.GetEntityManager().CreateEntity();
	CreateBox(cube3);
	TransformComponent& transform3 = cube3.Add<TransformComponent>([&] {
		return new TransformComponent(cube3);
	});
	transform3.SetLocalPosition(DirectX::SimpleMath::Vector3(5.0f, 0.0f, 0.0f));
	transform3.SetLocalScale(DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
	transform2.AddChild(cube3);
	RotationComponent& rotation3 = cube3.Add<RotationComponent>();
	rotation3.axis = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	rotation3.speed = 40.0f;

	ecs::Entity& cube4 = game.GetEntityManager().CreateEntity();
	CreateBox(cube4);
	TransformComponent& transform4 = cube4.Add<TransformComponent>([&] {
		return new TransformComponent(cube4);
	});
	transform4.SetLocalPosition(DirectX::SimpleMath::Vector3(3.0f, 0.0f, 0.0f));
	transform4.SetLocalScale(DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f));
	transform3.AddChild(cube4);
	RotationComponent& rotation4 = cube4.Add<RotationComponent>();
	rotation4.axis = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	rotation4.speed = 50.0f;

	game.Run();
	return 0;
}