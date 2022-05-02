#include <random>

#include "KatamariCameraSystem.h"
#include "KatamariControllerSystem.h"
#include "RenderPreparationSystem.h"
#include "ForwardRenderSystem.h"
#include "GeometryPassSystem.h"
#include "CollisionComponent.h"
#include "DirectionLightComponent.h"
#include "ItemComponent.h"
#include "DirectionLightSystem.h"
#include "ShadowMapDebugSystem.h"
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

engine::Model apricot_model;
engine::Model pear_model;
engine::Model pumpkin_model;
engine::Model apple_model;
engine::Model plane_model;

DirectX::ScratchImage apricot_diffuse;
DirectX::ScratchImage pear_diffuse;
DirectX::ScratchImage pumpkin_diffuse;
DirectX::ScratchImage apple_diffuse;
DirectX::ScratchImage plane_diffuse;

DirectX::ScratchImage apricot_normal;
DirectX::ScratchImage pear_normal;
DirectX::ScratchImage pumpkin_normal;
DirectX::ScratchImage apple_normal;
DirectX::ScratchImage plane_normal;

DirectX::ScratchImage apricot_specular;
DirectX::ScratchImage pear_specular;
DirectX::ScratchImage pumpkin_specular;
DirectX::ScratchImage apple_specular;
DirectX::ScratchImage plane_specular;

ecs::Entity& AttachModel(
	engine::Game& game,
	engine::Model& model,
	DirectX::ScratchImage& diffuse,
	DirectX::ScratchImage& normal,
	DirectX::ScratchImage& specular,
	Material material,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f },
	DirectX::SimpleMath::Vector3 euler = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& entity = game.GetEntityManager().CreateEntity();
	entity.Add<ModelComponent>([&] {
		return new ModelComponent(model, diffuse, normal, specular, material);
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

ecs::Entity& SpawnApricot(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	ecs::Entity& apricot = AttachModel(
		game,
		apricot_model,
		apricot_diffuse,
		apple_normal,
		apricot_specular,
		{ 0.1f, 3.0f, 0.5f },
		position,
		{ 0.3f, 0.3f, 0.3f },
		rotation);
	apricot.Add<ItemComponent>();
	AttachSphere(game, apricot, 2.0f, { 0.0f, 2.0f, 0.0f });
	return apricot;
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
		pear_normal,
		pear_specular,
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
		pumpkin_normal,
		pumpkin_specular,
		{ 0.1f, 32.0f, 0.75f },
		position,
		{ 3.0f, 3.0f, 3.0f },
		rotation);
	pumpkin.Add<ItemComponent>();
	AttachSphere(game, pumpkin, 0.5f, { 0.0f, 0.3f, 0.0f });
	return pumpkin;
}

void SpawnItems(engine::Game& game) {
	constexpr int radius = 7;
	constexpr int min = -radius;
	constexpr int max = 2 * radius;
	std::random_device dev;
	std::mt19937 rng(dev());
	const std::uniform_int_distribution<std::mt19937::result_type> dist(0, max);

	constexpr int pumpkin_count = 5;
	for (int i = 0; i < pumpkin_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnPumpkin(game, { static_cast<float>(x), -0.5f, static_cast<float>(y) });
	}

	constexpr int pear_count = 10;
	for (int i = 0; i < pear_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnPear(game, { static_cast<float>(x), 0.0f, static_cast<float>(y) });
	}

	constexpr int apricot_count = 5;
	for (int i = 0; i < apricot_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnApricot(game, { static_cast<float>(x), -0.5f, static_cast<float>(y) });
	}
}

ecs::Entity& SpawnCamera(engine::Game& game) {
	ecs::Entity& camera = game.GetEntityManager().CreateEntity();
	camera.Add<engine::CameraComponent>();
	camera.Add<engine::TransformComponent>();
	return camera;
}

ecs::Entity& SpawnKatamari(engine::Game& game, ecs::Entity& camera) {
	ecs::Entity& apple = AttachModel(
		game,
		apple_model,
		apple_diffuse,
		apple_normal,
		apple_specular,
		{ 0.1f, 32.0f, 0.6f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f });
	AttachSphere(game, apple, 1.0f);
	apple.Add<KatamariComponent>();

	ecs::Entity& katamari = game.GetEntityManager().CreateEntity();	
	katamari.Add<KatamariControllerComponent>([&] {
		return new KatamariControllerComponent(apple, 3.0f, 3.0f);
	});

	auto& katamari_transform = katamari.Add<engine::TransformComponent>();
	katamari_transform.SetPosition({ -9.0f, 0.0f, 0.0f });
	katamari_transform.AddChild(apple);
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

ecs::Entity& SpawnPlane(engine::Game& game, DirectX::SimpleMath::Vector3 position) {
	return AttachModel(
		game,
		plane_model,
		plane_diffuse,
		plane_normal,
		plane_specular,
		{ 0.1f, 10.0f, 0.1f },
		position,
		{ 15.0f, 1.0f, 15.0f },
		{ 0.0f, 0.0f, 0.0f });

}

HRESULT LoadModels() {
	using namespace engine;
	HRESULT result;

	if (result = Model::Load(apricot_model, "../Content/Apricot/Apricot.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(pear_model, "../Content/Pear/Pear.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(pumpkin_model, "../Content/Pumpkin/Pumpkin.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(apple_model, "../Content/Apple/Apple.obj"); FAILED(result)) {
		return result;
	}
	if (result = Model::Load(plane_model, "../Content/Plane/Plane.obj"); FAILED(result)) {
		return result;
	}

	return result;
}

HRESULT LoadDiffuse() {
	using namespace engine;
	HRESULT result;

	if (result = TextureLoader::LoadWic(L"../Content/Apricot/Apricot_Diffuse.png", &apricot_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Pear/Pear_Diffuse.jpg", &pear_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Pumpkin/Pumpkin_Diffuse.jpg", &pumpkin_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Apple/Apple_Diffuse.jpg", &apple_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Plane/Plane_Diffuse.jpg", &plane_diffuse); FAILED(result)) {
		return result;
	}

	return result;
}

HRESULT LoadNormal() {
	using namespace engine;
	HRESULT result;

	if (result = TextureLoader::LoadWic(L"../Content/Apricot/Apricot_Normal.png", &apricot_normal); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Pear/Pear_Normal.jpg", &pear_normal); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Pumpkin/Pumpkin_Normal.jpg", &pumpkin_normal); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Apple/Apple_Normal.jpg", &apple_normal); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Plane/Plane_Normal.jpg", &plane_normal); FAILED(result)) {
		return result;
	}

	return result;
}

HRESULT LoadSpecular() {
	using namespace engine;
	HRESULT result;

	if (result = TextureLoader::LoadWic(L"../Content/Apricot/Apricot_Specular.png", &apricot_specular); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Pear/Pear_Specular.jpg", &pear_specular); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Pumpkin/Pumpkin_Specular.jpg", &pumpkin_specular); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWic(L"../Content/Apple/Apple_Specular.jpg", &apple_specular); FAILED(result)) {
		return result;
	}
	// Use apple normals just because...
	if (result = TextureLoader::LoadWic(L"../Content/Apple/Apple_Specular.jpg", &plane_specular); FAILED(result)) {
		return result;
	} 

	return result;
}

void Release() {	
	apricot_diffuse.Release();
	pear_diffuse.Release();
	pumpkin_diffuse.Release();
	apple_diffuse.Release();
	plane_diffuse.Release();

	apricot_normal.Release();
	pear_normal.Release();
	pumpkin_normal.Release();
	apple_normal.Release();
	plane_normal.Release();

	apricot_specular.Release();
	pear_specular.Release();
	pumpkin_specular.Release();
	apple_specular.Release();
	plane_specular.Release();
}

int main() {
	HRESULT result;

	if (result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED); FAILED(result)) {
		return result;
	}
	if (result = LoadModels(); FAILED(result)) {
		return result;
	}
	if (result = LoadDiffuse(); FAILED(result)) {
		return result;
	}
	if (result = LoadNormal(); FAILED(result)) {
		return result;
	}
	if (result = LoadSpecular(); FAILED(result)) {
		return result;
	}

	using namespace engine;
	Game game;

	//CameraSystem fps_camera_system;
	KatamariCameraSystem camera_system;
	DirectionLightSystem direction_light_system;
	KatamariControllerSystem katamari_controller_system;
	StickingSystem sticking_system;
	RenderPreparationSystem render_preparation_system;
	LinesRendererSystem lines_renderer_system;
	ShadowMapRenderSystem shadow_system;
	ForwardRenderSystem render_system;
	ShadowMapDebugSystem shadow_debug_system;
	GeometryPassSystem geometry_pass_system;

	//game.PushSystem(fps_camera_system);
	game.PushSystem(camera_system);
	game.PushSystem(direction_light_system);
	game.PushSystem(katamari_controller_system);
	game.PushSystem(sticking_system);
	game.PushSystem(render_preparation_system);
	game.PushSystem(lines_renderer_system);
	game.PushSystem(shadow_system);
	game.PushSystem(render_system);
	game.PushSystem(shadow_debug_system);
	game.PushSystem(geometry_pass_system);

	auto& plane = DebugUtils::CreatePlane(game, 100, 100);
	auto& axis = DebugUtils::CreateAxis(game, 3.0f);

	DirectX::SimpleMath::Vector3 light_position = { 15.0f, 6.0f, 0.0f };
	DirectX::SimpleMath::Vector3 light_direction = {-1.0f, -1.0f, 0.0f };
	DirectX::SimpleMath::Vector3 light_color = { 1.0f, 1.0f, 1.0f };
	light_direction.Normalize();

	auto& camera = SpawnCamera(game);
	auto& light = SpawnDirectionLight(game, light_position, light_direction, light_color);
	auto& katamari = SpawnKatamari(game, camera);
	auto& grass = SpawnPlane(game, { 0.0f, -0.5f, 0.0f });
	SpawnItems(game);	

	game.Run();
	Release();
	return 0;
}