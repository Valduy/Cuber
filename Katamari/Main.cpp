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

	Model model;
	Model::Load(model, "C:/Users/Gleb/Desktop/Wooden Crate 01.obj");

	DirectX::ScratchImage texture;
	TextureLoader::LoadWic(
		L"C:/Users/Gleb/Desktop/Diffuse.png",
		&texture);

	ecs::Entity& entity = game.GetEntityManager().CreateEntity();
	entity.Add<ModelComponent>([&] {
		return new ModelComponent(model, texture);
	});
	entity.Add<TransformComponent>();

	game.Run();
	texture.Release();
	return 0;
}