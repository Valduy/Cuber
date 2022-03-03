#include <directxmath.h>
#include <vector>

#include "BorderSystem.h"
#include "RenderSystem.h"
#include "InputSystem.h"
#include "MoveSystem.h"
#include "ShapeComponent.h"
#include "BoundingBoxComponent.h"
#include "CollisionSystem.h"
#include "GoalSystem.h"
#include "TransformComponent.h"
#include "SpeedComponent.h"
#include "VelocityComponent.h"
#include "InputComponent.h"
#include "RestartSystem.h"
#include "VelocitySystem.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ShapeComponent& AddRectangleShape(ecs::Entity& entity, float width, float height) {
	ShapeComponent& shape_component = entity.Add<ShapeComponent>();
	shape_component.points = {
		DirectX::XMFLOAT4(-1.0f + width / 2, -1.0f - height / 2, 0.5f, 1.0f),
		DirectX::XMFLOAT4(-1.0f - width / 2,	-1.0f + height / 2, 0.5f, 1.0f),
		DirectX::XMFLOAT4(-1.0f + width / 2, -1.0f + height / 2, 0.5f, 1.0f),
		DirectX::XMFLOAT4(-1.0f - width / 2,	-1.0f - height / 2, 0.5f, 1.0f),
	};
	shape_component.indexes = {
		0, 1, 2,
		1, 0, 3,
	};

	return shape_component;
}

ecs::Entity& CreatePaddle(
	engine::Game& game,
	float x,
	float y,
	float width,
	float height,
	float speed,
	PlayerType player_type)
{
	ecs::Entity& paddle = game.GetEntityManager().AddEntity();

	ShapeComponent& shape_component = AddRectangleShape(paddle, width, height);
	shape_component.color = { 1, 1, 1, 1 };

	BoundingBoxComponent& bounding_box_component = paddle.Add<BoundingBoxComponent>();
	bounding_box_component.width = width;
	bounding_box_component.height = height;

	TransformComponent& transform_component = paddle.Add<TransformComponent>();
	transform_component.x = x;
	transform_component.y = y;

	SpeedComponent& speed_component = paddle.Add<SpeedComponent>();
	speed_component.speed = speed;

	InputComponent& input_component = paddle.Add<InputComponent>();
	input_component.player_type = player_type;
	input_component.up = false;
	input_component.down = false;

	return paddle;
}

ecs::Entity& CreateBall(
	engine::Game& game, 
	float x, 
	float y, 
	float size,
	float speed)
{
	ecs::Entity& ball = game.GetEntityManager().AddEntity();
	
	ShapeComponent& shape_component = AddRectangleShape(ball, size, size);
	shape_component.color = { 1, 1, 1, 1 };

	BoundingBoxComponent& bounding_box_component = ball.Add<BoundingBoxComponent>();
	bounding_box_component.width = size;
	bounding_box_component.height = size;

	TransformComponent& transform_component = ball.Add<TransformComponent>();
	transform_component.x = x;
	transform_component.y = y;

	SpeedComponent& speed_component = ball.Add<SpeedComponent>();
	speed_component.speed = speed;

	VelocityComponent& velocity_component = ball.Add<VelocityComponent>();
	velocity_component.x = -1.0f;
	velocity_component.y = 0.0f;

	return ball;
}

int main() {
	engine::Game game;
	RenderSystem render_system(game);
	CollisionSystem collision_system;
	BorderSystem border_system;
	VelocitySystem velocity_system;	
	InputSystem input_system;
	MoveSystem move_system;
	GoalSystem goal_system;
	RestartSystem restart_system;

	game.PushSystem(render_system);
	game.PushSystem(collision_system);
	game.PushSystem(border_system);
	game.PushSystem(velocity_system);	
	game.PushSystem(input_system);
	game.PushSystem(move_system);
	game.PushSystem(goal_system);
	game.PushSystem(restart_system);

	float field_width = 2.0f;
	float field_height = 2.0f;

	float paddle_speed = 1.0f;
	float paddle_width = 0.1f;
	float paddle_height = 0.4f;

	CreatePaddle(
		game, 
		0.1f + paddle_width / 2, 
		0.8f + paddle_height / 2, 
		paddle_width, 
		paddle_height, 
		paddle_speed, 
		PlayerType::kLeft);

	CreatePaddle(
		game, 
		1.8f + paddle_width / 2,
		0.8f + paddle_height / 2,
		paddle_width, 
		paddle_height, 
		paddle_speed, 
		PlayerType::kRight);

	float ball_size = 0.1f;
	float ball_speed = 1.0f;

	CreateBall(
		game, 
		1.0f, 
		1.0f, 
		ball_size, 
		ball_speed);

	game.Run();
	return 0;
}