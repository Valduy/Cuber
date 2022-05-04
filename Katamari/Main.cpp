#include <random>

#include "../Engine/Game.h"
#include "../Engine/Model.h"
#include "../Engine/DebugUtils.h"
#include "../Engine/TextureLoader.h"

#include "../Engine/LinesRenderSystem.h"

#include "KatamariCameraSystem.h"
#include "KatamariControllerSystem.h"
#include "ForwardRenderSystem.h"
#include "DefferedRenderSystem.h"
#include "UpdateLightSystem.h"
#include "ShadowMapDebugSystem.h"
#include "ShadowMapRenderSystem.h"
#include "StickingSystem.h"
#include "UpdateModelRenderDataSystem.h"

#include "CollisionComponent.h"
#include "DirectionLightComponent.h"
#include "ItemComponent.h"
#include "DnsMapsComponent.h"
#include "ModelComponent.h"
#include "MaterialComponent.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

engine::Model apricot_model;
engine::Model pear_model;
engine::Model pumpkin_model;
engine::Model apple_model;
engine::Model tile_model;

DirectX::ScratchImage apricot_diffuse;
DirectX::ScratchImage pear_diffuse;
DirectX::ScratchImage pumpkin_diffuse;
DirectX::ScratchImage apple_diffuse;
DirectX::ScratchImage tile_diffuse;

DirectX::ScratchImage apricot_normal;
DirectX::ScratchImage pear_normal;
DirectX::ScratchImage pumpkin_normal;
DirectX::ScratchImage apple_normal;
DirectX::ScratchImage tile_normal;

DirectX::ScratchImage apricot_specular;
DirectX::ScratchImage pear_specular;
DirectX::ScratchImage pumpkin_specular;
DirectX::ScratchImage apple_specular;
DirectX::ScratchImage tile_specular;

engine::TransformComponent& AddTransform(
	ecs::Entity& entity,
	DirectX::SimpleMath::Vector3 position,
	DirectX::SimpleMath::Vector3 euler = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f })
{
	auto& transform_component = entity.Add<engine::TransformComponent>();
	transform_component.SetPosition(position);
	transform_component.SetEuler(euler);
	transform_component.SetScale(scale);
	return transform_component;
}

ModelComponent& AttachModel(
	ecs::Entity& entity, 
	engine::Model& model)
{
	return entity.Add<ModelComponent>([&] {
		return new ModelComponent(model);
	});
}

MaterialComponent& AttachMaterial(ecs::Entity& entity, Material material) {
	auto& material_component = entity.Add<MaterialComponent>();
	material_component.material = material;
	return material_component;
}

DnsMapsComponent& AttachDnsMaps(
	ecs::Entity& entity,
	DirectX::ScratchImage& diffuse,
	DirectX::ScratchImage& normal, 
	DirectX::ScratchImage& specular)
{
	auto& maps_component = entity.Add<DnsMapsComponent>([&] {
		return new DnsMapsComponent(diffuse, normal, specular);
	});
	return maps_component;
}

ecs::Entity& AttachSphere(
	engine::Game& game, 
	ecs::Entity& entity,
	float radius,
	DirectX::SimpleMath::Vector3 local_position = { 0.0f, 0.0f, 0.0f })
{
	using namespace engine;
	auto& entity_transform = entity.Get<TransformComponent>();

	auto& sphere = DebugUtils::CreateSphere(game, 1.0f, { 0.0f, 1.0f, 0.0f, 1.0f });
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
	auto& apricot = game.GetEntityManager().CreateEntity();
	AddTransform(apricot, position, rotation, { 0.3f, 0.3f, 0.3f });
	AttachModel(apricot, apricot_model);
	AttachMaterial(apricot, { 0.1f, 3.0f, 0.5f });
	AttachDnsMaps(apricot, apricot_diffuse, apricot_normal, apricot_specular);
	AttachSphere(game, apricot, 2.0f, { 0.0f, 2.0f, 0.0f });
	apricot.Add<ItemComponent>();
	return apricot;
}

ecs::Entity& SpawnPear(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	auto& pear = game.GetEntityManager().CreateEntity();
	AddTransform(pear, position, rotation, { 0.3f, 0.3f, 0.3f });
	AttachModel(pear, pear_model);
	AttachMaterial(pear, { 0.1f, 32.0f, 0.75f });
	AttachDnsMaps(pear, pear_diffuse, pear_normal, pear_specular);
	AttachSphere(game, pear, 2.0f, { 0.0f, 0.0f, 0.0f });
	pear.Add<ItemComponent>();
	return pear;
}

ecs::Entity& SpawnPumpkin(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	auto& pumpkin = game.GetEntityManager().CreateEntity();
	AddTransform(pumpkin, position, rotation, { 3.0f, 3.0f, 3.0f });
	AttachModel(pumpkin, pumpkin_model);
	AttachMaterial(pumpkin, { 0.1f, 32.0f, 0.75f });
	AttachDnsMaps(pumpkin, pumpkin_diffuse, pumpkin_normal, pumpkin_specular);
	AttachSphere(game, pumpkin, 0.5f, { 0.0f, 0.3f, 0.0f });
	pumpkin.Add<ItemComponent>();
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
	auto& apple = game.GetEntityManager().CreateEntity();
	AddTransform(apple, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
	AttachModel(apple, apple_model);
	AttachMaterial(apple, { 0.1f, 32.0f, 0.6f });
	AttachDnsMaps(apple, apple_diffuse, apple_normal, apple_specular);
	AttachSphere(game, apple, 1.0f);
	apple.Add<KatamariComponent>();

	auto& katamari = game.GetEntityManager().CreateEntity();	
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

ecs::Entity& SpawnTile(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f })
{
	auto& tile = game.GetEntityManager().CreateEntity();
	AddTransform(tile, position, { 0.0f, 0.0f, 0.0f }, scale);
	AttachModel(tile, tile_model);
	AttachMaterial(tile, { 0.1f, 10.0f, 0.1f });
	AttachDnsMaps(tile, tile_diffuse, tile_normal, tile_specular);
	return tile;
}


ecs::Entity& SpawnPlane(engine::Game& game, DirectX::SimpleMath::Vector3 position) {
	using namespace DirectX::SimpleMath;
	auto& plane = game.GetEntityManager().CreateEntity();
	auto& plane_transform = AddTransform(plane, position);
	constexpr int ratio = 3;
	constexpr int half_ratio = ratio / 2;
	constexpr float tile_size = 4;
	
	for (int i = 0; i < ratio; ++i) {
		for (int j = 0; j < ratio; ++j) {
			const float local_x = tile_size * (i - half_ratio) * 2;
			const float local_z = tile_size * (j - half_ratio) * 2;
			auto& tile = SpawnTile(game, {}, { tile_size, 1.0f, tile_size });
			auto& tile_transform = tile.Get<engine::TransformComponent>();
			plane_transform.AddChild(tile);
			tile_transform.SetLocalPosition(Vector3{ local_x, 0, local_z });
		}		
	}

	return plane;
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
	if (result = Model::Load(tile_model, "../Content/Plane/Plane.obj"); FAILED(result)) {
		return result;
	}

	return result;
}

HRESULT LoadDiffuse() {
	using namespace engine;
	HRESULT result;

	if (result = TextureLoader::LoadWicSrgb(L"../Content/Apricot/Apricot_Diffuse.png", &apricot_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWicSrgb(L"../Content/Pear/Pear_Diffuse.jpg", &pear_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWicSrgb(L"../Content/Pumpkin/Pumpkin_Diffuse.jpg", &pumpkin_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWicSrgb(L"../Content/Apple/Apple_Diffuse.jpg", &apple_diffuse); FAILED(result)) {
		return result;
	}
	if (result = TextureLoader::LoadWicSrgb(L"../Content/Plane/Plane_Diffuse.jpg", &tile_diffuse); FAILED(result)) {
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
	if (result = TextureLoader::LoadWic(L"../Content/Plane/Plane_Normal.jpg", &tile_normal); FAILED(result)) {
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
	if (result = TextureLoader::LoadWic(L"../Content/Apple/Apple_Specular.jpg", &tile_specular); FAILED(result)) {
		return result;
	} 

	return result;
}

void Release() {	
	apricot_diffuse.Release();
	pear_diffuse.Release();
	pumpkin_diffuse.Release();
	apple_diffuse.Release();
	tile_diffuse.Release();

	apricot_normal.Release();
	pear_normal.Release();
	pumpkin_normal.Release();
	apple_normal.Release();
	tile_normal.Release();

	apricot_specular.Release();
	pear_specular.Release();
	pumpkin_specular.Release();
	apple_specular.Release();
	tile_specular.Release();
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
	UpdateLightSystem update_light_system;
	KatamariControllerSystem katamari_controller_system;
	StickingSystem sticking_system;
	UpdateModelRenderDataSystem update_model_render_data_system;
	LinesRendererSystem lines_renderer_system;
	ShadowMapRenderSystem shadow_system;
	ForwardRenderSystem forward_render_system;
	ShadowMapDebugSystem shadow_debug_system;
	DefferedRenderSystem deffered_render_system;

	//game.PushSystem(fps_camera_system);
	game.PushSystem(camera_system);
	game.PushSystem(update_light_system);
	game.PushSystem(katamari_controller_system);
	game.PushSystem(sticking_system);
	game.PushSystem(update_model_render_data_system);	
	//game.PushSystem(shadow_system);
	//game.PushSystem(forward_render_system);
	//game.PushSystem(shadow_debug_system);
	game.PushSystem(deffered_render_system);
	game.PushSystem(lines_renderer_system);

	auto& plane = DebugUtils::CreatePlane(game, 100, 100);
	auto& axis = DebugUtils::CreateAxis(game, 3.0f);

	DirectX::SimpleMath::Vector3 light_position = { 15.0f, 6.0f, 0.0f };
	DirectX::SimpleMath::Vector3 light_direction = {-1.0f, -2.0f, 0.0f };
	DirectX::SimpleMath::Vector3 light_color = { 1.0f, 1.0f, 1.0f };
	light_direction.Normalize();

	auto& camera = SpawnCamera(game);
	auto& light = SpawnDirectionLight(game, light_position, light_direction, light_color);
	auto& katamari = SpawnKatamari(game, camera);
	auto& grass = SpawnPlane(game, { 0.0f, -0.5f, 0.0f });
	SpawnItems(game);	

	//light_position = DirectX::SimpleMath::Vector3{ -15.0f, 6.0f, 0.0f };
	//light_direction = DirectX::SimpleMath::Vector3{ 1.0f, -2.0f, 0.0f };
	//light_direction.Normalize();
	//SpawnDirectionLight(game, light_position, light_direction, light_color);

	game.Run();
	Release();
	return 0;
}