#include <directxmath.h>
#include <vector>

#include "RenderSystem.h"
#include "InputSystem.h"
#include "MoveSystem.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"
#include "SpeedComponent.h"
#include "InputComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ecs::Entity& CreatePaddle(engine::Game& game, float x, float y, PlayerType player_type) {
	ecs::Entity& paddle = game.GetEntityManager().AddEntity();

	ShapeComponent& shape_component = paddle.Add<ShapeComponent>();
	shape_component.points = {
		DirectX::XMFLOAT4(-0.9f, -1.0f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(-1.0f, -0.6f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(-0.9f, -0.6f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(-1.0f, -1.0f, 0.5f, 1.0f),
	};
	shape_component.indexes = {
		0, 1, 2,
		1, 0, 3,
	};
	shape_component.color = { 1, 1, 1, 1 };

	TransformComponent& transform_component = paddle.Add<TransformComponent>();
	transform_component.x = x;
	transform_component.y = y;

	SpeedComponent& speed_component = paddle.Add<SpeedComponent>();
	speed_component.speed = 0.001;

	InputComponent& input_component = paddle.Add<InputComponent>();
	input_component.player_type = player_type;
	input_component.up = false;
	input_component.down = false;

	return paddle;
}

int main() {
	engine::Game game;
	RenderSystem render_system(game);
	InputSystem input_system;
	MoveSystem move_system;

	game.PushSystem(render_system);
	game.PushSystem(input_system);
	game.PushSystem(move_system);

	CreatePaddle(game, 0.1f, 0.8f, PlayerType::kLeft);
	CreatePaddle(game, 1.8f, 0.8f, PlayerType::kRight);

	game.Run();
	return 0;
}