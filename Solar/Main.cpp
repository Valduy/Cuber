#include <array>
#include <vector>

#include "../Engine/Game.h"
#include "../ECS/Entity.h"
#include "RenderSystem.h"
#include "LinesRenderSystem.h"
#include "RotationSystem.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "CameraSystem.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ShapeComponent& CreateBox(ecs::Entity& e) {
	ShapeComponent& shape_component = e.Add<ShapeComponent>();
	shape_component.points = {
		{ -0.5f,  0.5f, -0.5f }, // Top face
		{  0.5f,  0.5f, -0.5f },
		{  0.5f,  0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f },

		{ -0.5f, -0.5f,  0.5f }, // Bottom face
		{  0.5f, -0.5f,  0.5f },
		{  0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
	};
	shape_component.indexes = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		3, 2, 5,
		3, 5, 4,

		2, 1, 6,
		2, 6, 5,

		1, 7, 6,
		1, 0, 7,

		0, 3, 4,
		0, 4, 7
	};

	return shape_component;
}

using Lookup = std::map<std::pair<int, int>, int>;
using VertexList = std::vector<DirectX::SimpleMath::Vector3>;
using TriangleList = std::vector<int>;

int VertexForEdge(Lookup& lookup, VertexList& vertices, int first, int second)
{
	Lookup::key_type key(first, second);

	if (key.first > key.second) {
		std::swap(key.first, key.second);
	}

	const auto inserted = lookup.insert({ 
		key, vertices.size()
	});

	if (inserted.second)
	{
		DirectX::SimpleMath::Vector3& edge0 = vertices[first];
		DirectX::SimpleMath::Vector3& edge1 = vertices[second];
		DirectX::SimpleMath::Vector3 point = (edge0 + edge1);
		point.Normalize();
		vertices.push_back(point);
	}

	return inserted.first->second;
}

TriangleList Subdivide(VertexList& vertices, TriangleList triangles)
{
	Lookup lookup;
	TriangleList result;

	for (int i = 0; i < triangles.size(); i += 3) {
		std::array<int, 3> triangle = {
			triangles[i],
			triangles[i + 1],
			triangles[i + 2]
		};
		
		std::array<int, 3> mid;

		for (int edge = 0; edge < 3; ++edge)
		{
			mid[edge] = VertexForEdge(
				lookup, 
				vertices,
				triangle[edge], 
				triangle[(edge + 1) % 3]);
		}

		result.push_back(triangle[0]);
		result.push_back(mid[0]);
		result.push_back(mid[2]);

		result.push_back(triangle[1]);
		result.push_back(mid[1]);
		result.push_back(mid[0]);

		result.push_back(triangle[2]);
		result.push_back(mid[2]);
		result.push_back(mid[1]);

		result.push_back(mid[0]);
		result.push_back(mid[1]);
		result.push_back(mid[2]);
	}

	return result;
}

ShapeComponent& CreateSphere(ecs::Entity& e, int subdivisions) {
	const float x = 0.525731112119133606f;
	const float z = 0.850650808352039932f;
	const float n = 0.f;

	ShapeComponent& shape_component = e.Add<ShapeComponent>();
	shape_component.points = {
		{ -x, n, z }, {  x, n, z }, { -x, n,-z },
		{  x, n,-z }, {  n, z, x }, {  n, z,-x },
		{  n,-z, x }, {  n,-z,-x }, {  z, x, n },
		{ -z, x, n }, {  z,-x, n }, { -z,-x, n }
	};
	shape_component.indexes = {
		0, 4, 1,	0, 9, 4,	9, 5, 4,	4, 5, 8,	4, 8, 1,
		8, 10, 1,	8, 3, 10,	5, 3, 8,	5, 2, 3,	2, 7, 3,
		7, 10, 3,	7, 6, 10,	7, 11, 6,	11, 0, 6,	0, 1, 6,
		6, 1, 10,	9, 0, 11, 	9, 11, 2,	9, 2, 5, 	7, 2, 11
	};

	for (int i = 0; i < subdivisions; ++i)
	{
		shape_component.indexes = Subdivide(shape_component.points, shape_component.indexes);
	}

	return shape_component;
}

ecs::Entity& CreatePlanet(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 local_position, 
	DirectX::SimpleMath::Vector3 local_scale,
	float speed,
	DirectX::SimpleMath::Vector3 axis = { 0.0f, 1.0f, 0.0f })
{
	ecs::Entity& planet = game.GetEntityManager().CreateEntity();
	CreateSphere(planet, 5);

	TransformComponent& transform = planet.Add<TransformComponent>();
	transform.SetLocalPosition(local_position);
	transform.SetLocalScale(local_scale);

	RotationComponent& rotation = planet.Add<RotationComponent>();
	if (axis.Length() != 0) axis.Normalize();
	rotation.axis = axis;
	rotation.speed = speed;
	return planet;
}

ecs::Entity& CreatePlane(engine::Game& game, float size, int subdivisions) {
	ecs::Entity& plane = game.GetEntityManager().CreateEntity();
	plane.Add<TransformComponent>();
	LinesComponent& lines_component = plane.Add<LinesComponent>();

	float cell_size = size / static_cast<float>(subdivisions);
	float pivot_x = -size / 2;
	float pivot_z = -size / 2;

	using namespace DirectX::SimpleMath;
	for (int i = 0; i <= subdivisions; ++i) {
		const float x = pivot_x + cell_size * static_cast<float>(i);
		lines_component.points.push_back(Vector3(x, 0.0f, pivot_z));
		lines_component.points.push_back(Vector3(x, 0.0f, -pivot_z));
	}

	for (int i = 0; i <= subdivisions; ++i) {
		const float z = pivot_z + cell_size * static_cast<float>(i);
		lines_component.points.push_back(Vector3(pivot_x, 0.0f, z));
		lines_component.points.push_back(Vector3(-pivot_x, 0.0f, z));
	}

	return plane;
}

ecs::Entity& CreateLine(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 a, 
	DirectX::SimpleMath::Vector3 b,
	DirectX::SimpleMath::Vector4 color)
{
	using namespace DirectX::SimpleMath;
	ecs::Entity& line = game.GetEntityManager().CreateEntity();
	line.Add<TransformComponent>();
	LinesComponent& lines_component = line.Add<LinesComponent>();
	lines_component.color = color;
	lines_component.points = { a, b };

	return line;
}

ecs::Entity& CreateAxis(engine::Game& game, float length) {
	ecs::Entity& axis = game.GetEntityManager().CreateEntity();
	TransformComponent& transform = axis.Add<TransformComponent>();

	ecs::Entity& x = CreateLine(
		game,
		{ 0.0f, 0.0f, 0.0f },
		{ length, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f, 1.0f });
	transform.AddChild(x);

	ecs::Entity& y = CreateLine(
		game,
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, length, 0.0f },
		{ 0.0f, 1.0f, 0.0f, 1.0f });
	transform.AddChild(y);

	ecs::Entity& z = CreateLine(
		game,
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, length },
		{ 0.0f, 0.0f, 1.0f, 1.0f });
	transform.AddChild(z);

	return axis;
}

void SetParent(ecs::Entity& parent, ecs::Entity& child) {
	TransformComponent& parent_transform = parent.Get<TransformComponent>();
	parent_transform.AddChild(child);
}

int main() {
	engine::Game game;

	CameraSystem camera_system;
	RenderSystem render_system;
	LinesRendererSystem lines_renderer_system;
	RotationSystem rotation_system;

	game.PushSystem(camera_system);
	game.PushSystem(render_system);
	game.PushSystem(lines_renderer_system);
	game.PushSystem(rotation_system);	

	ecs::Entity& camera = game.GetEntityManager().CreateEntity();
	CameraComponent& camera_component = camera.Add<CameraComponent>();
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
		45.0f);
	SetParent(planet3, planet4);

	ecs::Entity& planet5 = CreatePlanet(
		game,
		Vector3(2.0f, 0.0f, 0.0f),
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

	ecs::Entity& plane = CreatePlane(game, 100, 100);
	ecs::Entity& axis = CreateAxis(game, 3.0f);

	game.Run();
	return 0;
}