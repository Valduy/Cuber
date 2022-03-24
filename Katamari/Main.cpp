#include "../Engine/Game.h"
#include "../Engine/Model.h"
#include "../Engine/DebugUtils.h"
#include "../Engine/CameraSystem.h"
#include "../Engine/LinesRenderSystem.h"
#include "../Engine/TextureLoader.h"
#include "RenderSystem.h"

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
	ModelComponent& model_component = entity.Add<ModelComponent>();
	engine::Model::Load(model_component.model, model_path);
	engine::TextureLoader::LoadWic(	texture_path, &model_component.texture);

	engine::TransformComponent& transform = entity.Add<engine::TransformComponent>();
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
	engine::TransformComponent& entity_transform = entity.Get<engine::TransformComponent>();
	ecs::Entity& sphere = engine::DebugUtils::CreateSphere(game, radius, { 0.0f, 1.0f, 0.0f, 1.0f });
	engine::TransformComponent& sphere_transform = sphere.Get<engine::TransformComponent>();
	entity_transform.AddChild(sphere);
	sphere_transform.SetLocalPosition(local_position);
	return sphere;
}

int main() {
	HRESULT result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(result)) return result;

	using namespace engine;
	Game game;

	CameraSystem camera_system;
	LinesRendererSystem lines_renderer_system;
	RenderSystem render_system;

	game.PushSystem(camera_system);
	game.PushSystem(lines_renderer_system);
	game.PushSystem(render_system);

	ecs::Entity& camera = game.GetEntityManager().CreateEntity();
	CameraComponent& camera_component = camera.Add<CameraComponent>();
	camera_component.position.z = -3.0f;

	ecs::Entity& plane = DebugUtils::CreatePlane(game, 100, 100);
	ecs::Entity& axis = DebugUtils::CreateAxis(game, 3.0f);

	ecs::Entity& crate = LoadModel(
		game,
		"../Content/WoodenCrate.obj",
		L"../Content/WoodenCrate_Diffuse.png");
	AttachSphere(game, crate, 1.0f, { 0.0f, 0.5f, 0.0f });

	ecs::Entity& mill = LoadModel(
		game,
		"../Content/WindMill.obj",
		L"../Content/WindMill_Diffuse.jpg",
		{ 3.0f, 0.0f, 0.0f });

	ecs::Entity& bag = LoadModel(
		game,
		"../Content/Bag.obj",
		L"../Content/Bag_Diffuse.jpg",
		{ -3.0f, 0.0f, 0.0f },
		{ 0.1f, 0.1f, 0.1f });

	ecs::Entity& pumpkin = LoadModel(
		game,
		"../Content/Pumpkin.obj",
		L"../Content/Pumpkin_Diffuse.png",
		{ -6.0f, 0.0f, 0.0f });
	
	ecs::Entity& ball = LoadModel(
		game,
		"../Content/SoccerBall.obj",
		L"../Content/SoccerBall_Diffuse.bmp",
		{ -9.0f, 0.0f, 0.0f },
		{ 0.2f, 0.2f, 0.2f });

	game.Run();
	return 0;
}