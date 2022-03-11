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

ecs::Entity& CreatePlanet(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 local_position, 
	DirectX::SimpleMath::Vector3 local_scale,
	float speed)
{
	ecs::Entity& planet = game.GetEntityManager().CreateEntity();
	CreateBox(planet);

	TransformComponent& transform = planet.Add<TransformComponent>([&] {
		return new TransformComponent(planet);
	});
	transform.SetLocalPosition(local_position);
	transform.SetLocalScale(local_scale);

	RotationComponent& rotation = planet.Add<RotationComponent>();
	rotation.axis = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	rotation.speed = speed;
	return planet;
}

void SetParent(ecs::Entity& parent, ecs::Entity& child) {
	TransformComponent& parent_transform = parent.Get<TransformComponent>();
	parent_transform.AddChild(child);
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

	using namespace DirectX::SimpleMath;
	ecs::Entity& planet1 = CreatePlanet(
		game, 
		Vector3::Zero, 
		Vector3(1.0f, 1.0f, 1.0f), 
		25.0f);

	ecs::Entity& planet2 = CreatePlanet(
		game, 
		Vector3(10.0f, 0.0f, 0.0f), 
		Vector3(0.75f, 0.75f, 0.75f), 
		45.0f);
	SetParent(planet1, planet2);

	ecs::Entity& planet3 = CreatePlanet(
		game,
		Vector3(5.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		45.0f);
	SetParent(planet2, planet3);

	ecs::Entity& planet4 = CreatePlanet(
		game,
		Vector3(3.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		45.0f);
	SetParent(planet3, planet4);

	ecs::Entity& planet5 = CreatePlanet(
		game,
		Vector3(2.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		45.0f);
	SetParent(planet4, planet5);

	ecs::Entity& planet6 = CreatePlanet(
		game,
		Vector3(30.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		85.0f);
	SetParent(planet1, planet6);

	ecs::Entity& planet7 = CreatePlanet(
		game,
		Vector3(2.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		45.0f);
	SetParent(planet6, planet7);

	game.Run();
	return 0;
}