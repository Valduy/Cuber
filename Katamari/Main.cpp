#include "../Engine/Game.h"
#include "../Engine/DebugUtils.h"
#include "../Engine/CameraSystem.h"
#include "../Engine/LinesRenderSystem.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

int main() {
	using namespace engine;
	Game game;

	CameraSystem camera_system;
	LinesRendererSystem lines_renderer_system;

	game.PushSystem(camera_system);
	game.PushSystem(lines_renderer_system);

	ecs::Entity& camera = game.GetEntityManager().CreateEntity();
	CameraComponent& camera_component = camera.Add<CameraComponent>();
	camera_component.position.z = -3.0f;

	ecs::Entity& plane = DebugUtils::CreatePlane(game, 100, 100);
	ecs::Entity& axis = DebugUtils::CreateAxis(game, 3.0f);

	game.Run();
	return 0;
}