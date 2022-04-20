#include <random>

#include "KatamariCameraSystem.h"
#include "KatamariControllerSystem.h"
#include "RenderSystem.h"
#include "CollisionComponent.h"
#include "DirectionLightComponent.h"
#include "ItemComponent.h"
#include "DirectionLightSystem.h"
#include "ShadowMapRenderSystem.h"
#include "StickingSystem.h"
#include "../Engine/Game.h"
#include "../Engine/Model.h"
#include "../Engine/DebugUtils.h"
#include "../Engine/LinesRenderSystem.h"
#include "../Engine/TextureLoader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

engine::Model crate_model;
engine::Model mill_model;
engine::Model pear_model;
engine::Model pumpkin_model;
engine::Model ball_model;
engine::Model grass_model;

DirectX::ScratchImage crate_diffuse;
DirectX::ScratchImage mill_diffuse;
DirectX::ScratchImage pear_diffuse;
DirectX::ScratchImage pumpkin_diffuse;
DirectX::ScratchImage ball_diffuse;
DirectX::ScratchImage grass_diffuse;

ecs::Entity& AttachModel(
	engine::Game& game,
	engine::Model& model,
	DirectX::ScratchImage& texture,
	Material material,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f },
	DirectX::SimpleMath::Vector3 euler = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& entity = game.GetEntityManager().CreateEntity();
	entity.Add<ModelComponent>([&] {
		return new ModelComponent(model, texture, material);
	});

	auto& transform = entity.Add<engine::TransformComponent>();
	transform.SetPosition(position);
	transform.SetScale(scale);
	transform.SetEuler(euler);

	return entity;
}

ecs::Entity& AttachSphere(
	engine::Game& game, 
	ecs::Entity& entity,
	float radius,
	DirectX::SimpleMath::Vector3 local_position = { 0.0f, 0.0f, 0.0f })
{
	using namespace engine;
	auto& entity_transform = entity.Get<TransformComponent>();

	ecs::Entity& sphere = DebugUtils::CreateSphere(game, 1.0f, { 0.0f, 1.0f, 0.0f, 1.0f });
	auto& sphere_transform = sphere.Get<TransformComponent>();
	entity_transform.AddChild(sphere);
	sphere_transform.SetLocalPosition(local_position);
	sphere_transform.SetLocalScale({ radius, radius, radius });

	entity.Add<CollisionComponent>([&] {
		return new CollisionComponent(sphere, local_position, radius);
	});

	return sphere;
}

ecs::Entity& SpawnCrate(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& crate = AttachModel(
		game,
		crate_model,
		crate_diffuse,
		{ 0.1f, 3.0f, 0.5f },
		position,
		{ 0.7f, 0.7f, 0.7f },
		rotation);
	crate.Add<ItemComponent>();
	AttachSphere(game, crate, 1.0f, { 0.0f, 0.5f, 0.0f });
	return crate;
}

ecs::Entity& SpawnMill(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& mill = AttachModel(
		game,
		mill_model,
		mill_diffuse,
		{ 0.1f, 3.0f, 0.5f },
		position,
		{ 0.3f, 0.3f, 0.3f },
		rotation);
	mill.Add<ItemComponent>();
	AttachSphere(game, mill, 4.0f, { 0.0f, 2.0f, 0.0f });
	return mill;
}

ecs::Entity& SpawnPear(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& pear = AttachModel(
		game,
		pear_model,
		pear_diffuse,
		{ 0.1f, 32.0f, 0.75f },
		position,
		{ 0.3f, 0.3f, 0.3f },
		rotation);
	pear.Add<ItemComponent>();
	AttachSphere(game, pear, 2.0f, { 0.0f, 0.0f, 0.0f });
	return pear;
}

ecs::Entity& SpawnPumpkin(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& pumpkin = AttachModel(
		game,
		pumpkin_model,
		pumpkin_diffuse,
		{ 0.1f, 32.0f, 0.75f },
		position,
		{ 3.0f, 3.0f, 3.0f },
		rotation);
	pumpkin.Add<ItemComponent>();
	AttachSphere(game, pumpkin, 0.5f, { 0.0f, 0.3f, 0.0f });
	return pumpkin;
}

void SpawnItems(engine::Game& game) {
	constexpr int radius = 10;
	constexpr int min = -radius;
	constexpr int max = 2 * radius;
	std::random_device dev;
	std::mt19937 rng(dev());
	const std::uniform_int_distribution<std::mt19937::result_type> dist(0, max);

	constexpr int pumpkin_count = 5;
	for (int i = 0; i < pumpkin_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnPumpkin(game, { static_cast<float>(x), 0.0f, static_cast<float>(y) });
	}

	constexpr int pear_count = 10;
	for (int i = 0; i < pear_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnPear(game, { static_cast<float>(x), 0.0f, static_cast<float>(y) });
	}

	constexpr int crate_count = 5;
	for (int i = 0; i < crate_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnCrate(game, { static_cast<float>(x), 0.0f, static_cast<float>(y) });
	}

	constexpr int mill_count = 5;
	for (int i = 0; i < mill_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnMill(game, { static_cast<float>(x), 0.0f, static_cast<float>(y) });
	}
}

ecs::Entity& SpawnCamera(engine::Game& game) {
	ecs::Entity& camera = game.GetEntityManager().CreateEntity();
	camera.Add<engine::CameraComponent>();
	camera.Add<engine::TransformComponent>();
	return camera;
}

ecs::Entity& SpawnKatamari(engine::Game& game, ecs::Entity& camera) {
	ecs::Entity& ball = AttachModel(
		game,
		ball_model,
		ball_diffuse,
		{ 0.1f, 32.0f, 0.6f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.25f, 0.25f, 0.25f });
	AttachSphere(game, ball, 3.2f);
	ball.Add<KatamariComponent>();

	ecs::Entity& katamari = game.GetEntityManager().CreateEntity();	
	katamari.Add<KatamariControllerComponent>([&] {
		return new KatamariControllerComponent(ball, 3.0f, 3.0f);
	});

	auto& katamari_transform = katamari.Add<engine::TransformComponent>();
	katamari_transform.SetPosition({ -9.0f, 0.0f, 0.0f });
	katamari_transform.AddChild(ball);
	katamari_transform.AddChild(camera);
	camera.Get<engine::TransformComponent>().SetLocalPosition({ 0.0, 5.0, -10.0 });
	return katamari;
}

ecs::Entity& SpawnDirectionLight(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 position, 
	DirectX::SimpleMath::Vector3 direction,
	DirectX::SimpleMath::Vector3 color)
{
	auto& light = game.GetEntityManager().CreateEntity();
	auto& light_transform = light.Add<engine::TransformComponent>();
	light_transform.SetPosition(position);

	auto& axis = engine::DebugUtils::CreateAxis(game, 1);
	light_transform.AddChild(axis);

	auto& sphere = engine::DebugUtils::CreateSphere(game, 1, { 1.0f, 1.0f, 1.0f, 0.0f });
	light_transform.AddChild(sphere);

	auto& line = engine::DebugUtils::CreateLine(game, position, position + direction, { 1.0f, 1.0f, 0.0f, 0.0f });

	auto& direction_light = light.Add<DirectionLightComponent>();
	direction_light.light_direction = direction;
	direction_light.light_color = color;
	return light;
}

ecs::Entity& SpawnPlane(engine::Game& game) {
	ecs::Entity& plane = AttachModel(
		game,
		grass_model,
		grass_diffuse,
		{ 0.1f, 10.0f, 0.1f },
		{ 0.0f, -1.0f, 0.0f },
		{ 20.0f, 1.0f, 20.0f },
		{ 0.0f, 0.0f, 0.0f });
	return plane;
}

HRESULT LoadModels() {
	using namespace engine;
	HRESULT result;

	if (result = Model::Load(crate_model, "../Content/WoodenCrate.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(mill_model, "../Content/WindMill.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(pear_model, "../Content/Pear.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(pumpkin_model, "../Content/Pumpkin.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(ball_model, "../Content/SoccerBall.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(grass_model, "../Content/Plane.obj"); FAILED(result)) {
		return result;
	}

	return result;
}

HRESULT LoadTextures() {
	using namespace engine;
	HRESULT result;

	if (result = TextureLoader::LoadWic(L"../Content/WoodenCrate_Diffuse.png", &crate_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/WindMill_Diffuse.jpg", &mill_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Pear_Diffuse.jpg", &pear_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Pumpkin_Diffuse.jpg", &pumpkin_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/SoccerBall_Diffuse.bmp", &ball_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Plane_Diffuse.jpg", &grass_diffuse); FAILED(result)) {
		return result;
	}

	return result;
}

void Release() {
	crate_diffuse.Release();
	mill_diffuse.Release();
	pear_diffuse.Release();
	pumpkin_diffuse.Release();
	pear_diffuse.Release();
}

int main() {
	HRESULT result;

	if (result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED); FAILED(result)) {
		return result;
	}
	if (result = LoadModels(); FAILED(result)) {
		return result;
	}
	if (result = LoadTextures(); FAILED(result)) {
		return result;
	}

	using namespace engine;
	Game game;

	//CameraSystem fps_camera_system;
	KatamariCameraSystem camera_system;
	DirectionLightSystem direction_light_system;
	KatamariControllerSystem katamari_controller_system;
	StickingSystem sticking_system;
	LinesRendererSystem lines_renderer_system;
	ShadowMapRenderSystem shadow_system;
	RenderSystem render_system;

	//game.PushSystem(fps_camera_system);
	game.PushSystem(camera_system);
	game.PushSystem(direction_light_system);
	game.PushSystem(katamari_controller_system);
	game.PushSystem(sticking_system);
	game.PushSystem(lines_renderer_system);
	game.PushSystem(shadow_system);
	game.PushSystem(render_system);	

	auto& plane = DebugUtils::CreatePlane(game, 100, 100);
	auto& axis = DebugUtils::CreateAxis(game, 3.0f);

	DirectX::SimpleMath::Vector3 light_position = { 25.0f, 6.0f, 0.0f };
	DirectX::SimpleMath::Vector3 light_direction = {-1.0f, -1.0f, 0.0f };
	DirectX::SimpleMath::Vector3 light_color = { 1.0f, 1.0f, 1.0f };
	light_direction.Normalize();

	auto& camera = SpawnCamera(game);
	auto& light = SpawnDirectionLight(game, light_position, light_direction, light_color);
	auto& katamari = SpawnKatamari(game, camera);
	auto& grass = SpawnPlane(game);
	SpawnItems(game);	

	game.Run();
	return 0;
}