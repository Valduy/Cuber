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
	const wchar_t* texture_path)
{
	ecs::Entity& entity = game.GetEntityManager().CreateEntity();
	ModelComponent& model_component = entity.Add<ModelComponent>();
	engine::Model::Load(model_component.model, model_path);
	engine::TextureLoader::LoadWic(	texture_path, &model_component.texture);
	return entity;
}

int main() {
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

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
	crate.Add<TransformComponent>();

	ecs::Entity& mill = LoadModel(
		game,
		"../Content/WindMill.obj",
		L"../Content/WindMill_Diffuse.jpg");
	TransformComponent& mill_transform = mill.Add<TransformComponent>();
	mill_transform.SetPosition({ 3.0f, 0.0f, 0.0f });

	ecs::Entity& bag = LoadModel(
		game,
		"../Content/Bag.obj",
		L"../Content/Bag_Diffuse.jpg");
	TransformComponent& bag_transform = bag.Add<TransformComponent>();
	bag_transform.SetPosition({ -3.0f, 0.0f, 0.0f });
	bag_transform.SetScale({ 0.1f, 0.1f, 0.1f });

	ecs::Entity& pumpkin = LoadModel(
		game,
		"../Content/Pumpkin.obj",
		L"../Content/Pumpkin_Diffuse.png");
	TransformComponent& pumpkin_transform = pumpkin.Add<TransformComponent>();
	pumpkin_transform.SetPosition({ -6.0f, 0.0f, 0.0f });

	game.Run();
	return 0;
}