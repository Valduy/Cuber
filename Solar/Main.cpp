#include "../Engine/Game.h"
#include "../Engine/DebugUtils.h"
#include "../Engine/LinesRenderSystem.h"
#include "../Engine/CameraSystem.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/CameraComponent.h"
#include "../ECS/Entity.h"
#include "RenderSystem.h"
#include "RotationSystem.h"
#include "SinSystem.h"
#include "ShapeComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ecs::Entity& CreatePlanet(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 local_position, 
	DirectX::SimpleMath::Vector3 local_scale,
	float speed,
	DirectX::SimpleMath::Vector3 axis = { 0.0f, 1.0f, 0.0f })
{
	ecs::Entity& planet = game.GetEntityManager().CreateEntity();
	planet.Add<ShapeComponent>([&] {
		return ShapeComponent::CreateSphere(5);
	});

	engine::TransformComponent& transform = planet.Add<engine::TransformComponent>();
	transform.SetLocalPosition(local_position);
	transform.SetLocalScale(local_scale);

	RotationComponent& rotation = planet.Add<RotationComponent>();
	if (axis.Length() != 0) axis.Normalize();
	rotation.axis = axis;
	rotation.speed = speed;
	return planet;
}

void SetParent(ecs::Entity& parent, ecs::Entity& child) {
	engine::TransformComponent& parent_transform = parent.Get<engine::TransformComponent>();
	parent_transform.AddChild(child);
}

int main() {
	engine::Game game;

	engine::CameraSystem camera_system;
	RenderSystem render_system;
	engine::LinesRendererSystem lines_renderer_system;
	RotationSystem rotation_system;
	SinSystem sin_system;

	game.PushSystem(camera_system);
	game.PushSystem(render_system);
	game.PushSystem(lines_renderer_system);
	game.PushSystem(rotation_system);
	game.PushSystem(sin_system);

	ecs::Entity& camera = game.GetEntityManager().CreateEntity();
	engine::CameraComponent& camera_component = camera.Add<engine::CameraComponent>();
	camera_component.position.z = -3.0f;

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
		Vector3(5.0f, 0.0f, -5.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		45.0f);
	SetParent(planet2, planet3);

	ecs::Entity& planet4 = CreatePlanet(
		game,
		Vector3(3.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		120.0f,
		Vector3(1.0f, 0.0f, 0.0f));
	SetParent(planet3, planet4);

	ecs::Entity& planet5 = CreatePlanet(
		game,
		Vector3(0.0f, 2.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		45.0f);
	SetParent(planet4, planet5);

	ecs::Entity& planet6 = CreatePlanet(
		game,
		Vector3(30.0f, 0.0f, 10.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		85.0f);
	SetParent(planet1, planet6);
	
	ecs::Entity& planet7 = CreatePlanet(
		game,
		Vector3(2.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		100.0f,
		Vector3(1.0f, 0.0f, 0.0f));
	SetParent(planet6, planet7);

	ecs::Entity& planet8 = CreatePlanet(
		game,
		Vector3(0.0f, 2.0f, 0.0f),
		Vector3(0.5f, 0.5f, 0.5f),
		50.0f,
		Vector3(0.0f, 1.0f, 0.0f));
	SetParent(planet7, planet8);

	ecs::Entity& plane = engine::DebugUtils::CreatePlane(game, 100, 100);
	ecs::Entity& axis = engine::DebugUtils::CreateAxis(game, 3.0f);

	game.Run();
	return 0;
}