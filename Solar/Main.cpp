#include "../Engine/Game.h"
#include "../Engine/DebugUtils.h"
#include "../Engine/LinesRenderSystem.h"
#include "../Engine/CameraSystem.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/CameraComponent.h"
#include "../Ash/Entity.h"
#include "RenderSystem.h"
#include "RotationSystem.h"
#include "SinSystem.h"
#include "ShapeComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ash::Entity& CreatePlanet(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 local_position, 
	DirectX::SimpleMath::Vector3 local_scale,
	float speed,
	DirectX::SimpleMath::Vector3 rotation_axis = { 0.0f, 1.0f, 0.0f })
{
	ash::Entity& planet = game.GetEntities().Create();
	planet.Add<ShapeComponent>([&] {
		return ShapeComponent::CreateSphere(5);
	});
	
	engine::TransformComponent& transform = planet.Add<engine::TransformComponent>();
	transform.SetLocalPosition(local_position);
	transform.SetLocalScale(local_scale);

	ash::Entity& axis = engine::DebugUtils::CreateAxis(game, 3.0f);
	axis.Get<engine::TransformComponent>().SetParent(planet);

	RotationComponent& rotation = planet.Add<RotationComponent>();
	if (rotation_axis.Length() != 0) rotation_axis.Normalize();
	rotation.axis = rotation_axis;
	rotation.speed = speed;
	return planet;
}

int main() {
	using namespace engine;
	using namespace DirectX::SimpleMath;

	Game game;

	CameraSystem camera_system;
	RenderSystem render_system;
	LinesRendererSystem lines_renderer_system;
	RotationSystem rotation_system;
	SinSystem sin_system;

	game.PushSystem(camera_system);
	game.PushSystem(render_system);
	game.PushSystem(lines_renderer_system);
	game.PushSystem(rotation_system);
	game.PushSystem(sin_system);

	ash::Entity& camera = game.GetEntities().Create();
	CameraComponent& camera_component = camera.Add<CameraComponent>();
	camera_component.position.z = -3.0f;

	ash::Entity& plane = DebugUtils::CreatePlane(game, 100, 100);
	ash::Entity& axis = DebugUtils::CreateAxis(game, 3.0f);
		
	ash::Entity& planet1 = CreatePlanet(
		game, 
		Vector3::Zero, 
		Vector3(1.0f, 1.0f, 1.0f), 
		1.0f);

	ash::Entity& planet2 = CreatePlanet(
		game, 
		Vector3(10.0f, 0.0f, 0.0f), 
		Vector3(0.75f, 0.75f, 0.75f), 
		2.0f);
	planet2.Get<engine::TransformComponent>().SetParent(planet1);

	ash::Entity& planet3 = CreatePlanet(
		game,
		Vector3(5.0f, 0.0f, -5.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		2.0f);
	planet3.Get<engine::TransformComponent>().SetParent(planet2);

	ash::Entity& planet4 = CreatePlanet(
		game,
		Vector3(3.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		2.0f,
		Vector3(1.0f, 0.0f, 0.0f));
	planet4.Get<engine::TransformComponent>().SetParent(planet3);

	ash::Entity& planet5 = CreatePlanet(
		game,
		Vector3(0.0f, 2.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		2.0f);
	planet5.Get<engine::TransformComponent>().SetParent(planet4);

	ash::Entity& planet6 = CreatePlanet(
		game,
		Vector3(30.0f, 0.0f, 10.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		3.0f);
	planet6.Get<engine::TransformComponent>().SetParent(planet1);
	
	ash::Entity& planet7 = CreatePlanet(
		game,
		Vector3(2.0f, 0.0f, 0.0f),
		Vector3(0.75f, 0.75f, 0.75f),
		1.0f,
		Vector3(1.0f, 0.0f, 0.0f));
	planet7.Get<engine::TransformComponent>().SetParent(planet6);

	ash::Entity& planet8 = CreatePlanet(
		game,
		Vector3(0.0f, 2.0f, 0.0f),
		Vector3(0.5f, 0.5f, 0.5f),
		2.0f,
		Vector3(0.0f, 1.0f, 0.0f));
	planet8.Get<engine::TransformComponent>().SetParent(planet7);

	game.Run();
	return 0;
}