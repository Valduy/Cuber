#include <random>

#include "KatamariCameraSystem.h"
#include "KatamariControllerSystem.h"
#include "RenderSystem.h"
#include "CollisionComponent.h"
#include "ItemComponent.h"
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

ecs::Entity& LoadModel(
	engine::Game& game, 
	const std::string& model_path, 
	const wchar_t* texture_path,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f },
	DirectX::SimpleMath::Vector3 euler = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& entity = game.GetEntityManager().CreateEntity();
	auto& model_component = entity.Add<ModelComponent>();
	engine::Model::Load(model_component.model, model_path);
	engine::TextureLoader::LoadWic(	texture_path, &model_component.texture);

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

	auto& collision_component = entity.Add<CollisionComponent>([&] {
		return new CollisionComponent(sphere, local_position, radius);
	});

	return sphere;
}

ecs::Entity& SpawnCrate(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& crate = LoadModel(
		game,
		"../Content/WoodenCrate.obj",
		L"../Content/WoodenCrate_Diffuse.png",
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
	ecs::Entity& mill = LoadModel(
		game,
		"../Content/WindMill.obj",
		L"../Content/WindMill_Diffuse.jpg",
		position,
		{ 0.3f, 0.3f, 0.3f },
		rotation);
	mill.Add<ItemComponent>();
	AttachSphere(game, mill, 4.0f, { 0.0f, 2.0f, 0.0f });
	return mill;
}

ecs::Entity& SpawnBag(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& bag = LoadModel(
		game,
		"../Content/Bag.obj",
		L"../Content/Bag_Diffuse.jpg",
		position,
		{ 0.07f, 0.07f, 0.07f },
		rotation);
	bag.Add<ItemComponent>();
	AttachSphere(game, bag, 6.0f, { 0.0f, -1.0f, 0.0f });
	return bag;
}

ecs::Entity& SpawnPumpkin(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& pumpkin = LoadModel(
		game,
		"../Content/Pumpkin.obj",
		L"../Content/Pumpkin_Diffuse.png",
		position,
		{ 1.0f, 1.0f, 1.0f },
		rotation);
	pumpkin.Add<ItemComponent>();
	AttachSphere(game, pumpkin, 0.4f, { 0.0f, 0.2f, 0.0f });
	return pumpkin;
}

void SpawnItems(engine::Game& game) {
	constexpr int radius = 10;
	constexpr int min = -radius;
	constexpr int max = 2 * radius;
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, max);

	constexpr int pumpkin_count = 5;
	for (int i = 0; i < pumpkin_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnPumpkin(game, { static_cast<float>(x), 0.0f, static_cast<float>(y) });
	}

	constexpr int bag_count = 5;
	for (int i = 0; i < bag_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnBag(game, { static_cast<float>(x), 0.0f, static_cast<float>(y) });
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

int main() {
	HRESULT result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(result)) return result;

	using namespace engine;
	Game game;

	KatamariCameraSystem camera_system;
	KatamariControllerSystem katamari_controller_system;
	StickingSystem sticking_system;
	LinesRendererSystem lines_renderer_system;
	RenderSystem render_system;

	game.PushSystem(camera_system);
	game.PushSystem(katamari_controller_system);
	game.PushSystem(sticking_system);
	game.PushSystem(lines_renderer_system);
	game.PushSystem(render_system);

	ecs::Entity& plane = DebugUtils::CreatePlane(game, 100, 100);
	ecs::Entity& axis = DebugUtils::CreateAxis(game, 3.0f);

	ecs::Entity& camera = game.GetEntityManager().CreateEntity();
	camera.Add<CameraComponent>();
	camera.Add<TransformComponent>();

	SpawnItems(game);

	ecs::Entity& ball = LoadModel(
		game,
		"../Content/SoccerBall.obj",
		L"../Content/SoccerBall_Diffuse.bmp",
		{ 0.0f, 0.0f, 0.0f },
		{ 0.2f, 0.2f, 0.2f });
	AttachSphere(game, ball, 3.0f);
	ball.Add<KatamariComponent>();

	ecs::Entity& katamari = game.GetEntityManager().CreateEntity();
	TransformComponent& katamari_transform = katamari.Add<TransformComponent>();
	katamari.Add<KatamariControllerComponent>([&] {
		return new KatamariControllerComponent(ball, 3.0f, 3.0f);
	});
	katamari_transform.SetPosition({ -9.0f, 0.0f, 0.0f });
	katamari_transform.AddChild(ball);
	katamari_transform.AddChild(camera);
	camera.Get<TransformComponent>().SetLocalPosition({ 0.0, 5.0, -10.0 });

	game.Run();
	return 0;
}