#include <DirectXMath.h>
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
#include "AmbientLightComponent.h";

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

engine::Model icosphere_model;

engine::Model apricot_model;
engine::Model pear_model;
engine::Model statuette_model;
engine::Model apple_model;
engine::Model tile_model;

DirectX::ScratchImage apricot_diffuse;
DirectX::ScratchImage pear_diffuse;
DirectX::ScratchImage statuette_diffuse;
DirectX::ScratchImage apple_diffuse;
DirectX::ScratchImage tile_diffuse;

DirectX::ScratchImage apricot_normal;
DirectX::ScratchImage pear_normal;
DirectX::ScratchImage statuette_normal;
DirectX::ScratchImage apple_normal;
DirectX::ScratchImage tile_normal;

DirectX::ScratchImage apricot_specular;
DirectX::ScratchImage pear_specular;
DirectX::ScratchImage statuette_specular;
DirectX::ScratchImage apple_specular;
DirectX::ScratchImage tile_specular;

engine::TransformComponent& AddTransform(
	ash::Entity& entity,
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
	ash::Entity& entity, 
	engine::Model& model)
{
	return entity.Add<ModelComponent>([&] {
		return new ModelComponent(model);
	});
}

MaterialComponent& AttachMaterial(ash::Entity& entity, Material material) {
	auto& material_component = entity.Add<MaterialComponent>();
	material_component.material = material;
	return material_component;
}

DnsMapsComponent& AttachDnsMaps(
	ash::Entity& entity,
	DirectX::ScratchImage& diffuse,
	DirectX::ScratchImage& normal, 
	DirectX::ScratchImage& specular)
{
	auto& maps_component = entity.Add<DnsMapsComponent>([&] {
		return new DnsMapsComponent(diffuse, normal, specular);
	});
	return maps_component;
}

ash::Entity& AttachSphere(
	engine::Game& game, 
	ash::Entity& entity,
	float radius,
	DirectX::SimpleMath::Vector3 local_position = { 0.0f, 0.0f, 0.0f })
{
	using namespace engine;

	auto& sphere = DebugUtils::CreateSphere(game, 1.0f, { 0.0f, 1.0f, 0.0f, 1.0f });
	auto& sphere_transform = sphere.Get<TransformComponent>();
	sphere_transform.SetParent(entity);
	sphere_transform.SetLocalPosition(local_position);
	sphere_transform.SetLocalScale({ radius, radius, radius });

	entity.Add<CollisionComponent>([&] {
		return new CollisionComponent(sphere, local_position, radius);
	});

	return sphere;
}

ash::Entity& SpawnApricot(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	auto& apricot = game.GetEntities().Create();
	AddTransform(apricot, position, rotation, { 0.3f, 0.3f, 0.3f });
	AttachModel(apricot, apricot_model);
	AttachMaterial(apricot, { 0.1f, 3.0f, 0.5f });
	AttachDnsMaps(apricot, apricot_diffuse, apricot_normal, apricot_specular);
	AttachSphere(game, apricot, 2.0f, { 0.0f, 2.0f, 0.0f });
	apricot.Add<ItemComponent>();
	return apricot;
}

ash::Entity& SpawnPear(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	auto& pear = game.GetEntities().Create();
	AddTransform(pear, position, rotation, { 0.3f, 0.3f, 0.3f });
	AttachModel(pear, pear_model);
	AttachMaterial(pear, { 0.1f, 32.0f, 0.75f });
	AttachDnsMaps(pear, pear_diffuse, pear_normal, pear_specular);
	AttachSphere(game, pear, 2.0f, { 0.0f, 0.0f, 0.0f });
	pear.Add<ItemComponent>();
	return pear;
}

ash::Entity& SpawnStatuette(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f })
{
	auto& statuette = game.GetEntities().Create();
	AddTransform(statuette, position, rotation, { 0.3f, 0.3f, 0.3f });
	AttachModel(statuette, statuette_model);
	AttachMaterial(statuette, { 0.1f, 32.0f, 0.75f });
	AttachDnsMaps(statuette, statuette_diffuse, statuette_normal, statuette_specular);
	AttachSphere(game, statuette, 5.0f, { 0.0f, 5.0f, 0.0f });
	statuette.Add<ItemComponent>();
	return statuette;
}

void SpawnItems(engine::Game& game) {
	constexpr int radius = 7;
	constexpr int min = -radius;
	constexpr int max = 2 * radius;
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, max);

	constexpr int statuette_count = 5;
	for (int i = 0; i < statuette_count; ++i) {
		int x = min + dist(rng);
		int y = min + dist(rng);
		SpawnStatuette(game, { static_cast<float>(x), -0.5f, static_cast<float>(y) });
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

ash::Entity& SpawnCamera(engine::Game& game) {
	auto& camera = game.GetEntities().Create();
	camera.Add<engine::CameraComponent>();
	camera.Add<engine::TransformComponent>();
	return camera;
}

ash::Entity& SpawnKatamari(engine::Game& game, ash::Entity& camera) {
	auto& apple = game.GetEntities().Create();
	auto& apple_transform = AddTransform(apple, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
	AttachModel(apple, apple_model);
	AttachMaterial(apple, { 0.1f, 32.0f, 0.6f });
	AttachDnsMaps(apple, apple_diffuse, apple_normal, apple_specular);
	AttachSphere(game, apple, 1.0f);
	apple.Add<KatamariComponent>();

	auto& katamari = game.GetEntities().Create();	
	katamari.Add<KatamariControllerComponent>([&] {
		return new KatamariControllerComponent(apple, 3.0f, 3.0f);
	});

	auto& katamari_transform = katamari.Add<engine::TransformComponent>();
	katamari_transform.SetPosition({ -9.0f, 0.0f, 0.0f });

	apple_transform.SetParent(katamari);

	auto& camera_transform = camera.Get<engine::TransformComponent>();
	camera_transform.SetParent(katamari);
	camera_transform.SetLocalPosition({ 0.0, 5.0, -10.0 });

	return katamari;
}

ash::Entity& SpawnAmbientLight(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 color, 
	float intensity)
{
	auto& light = game.GetEntities().Create();
	auto& ambient_light = light.Add<AmbientLightComponent>();
	ambient_light.light_color = color;
	ambient_light.intensity = intensity;
	return light;
}

ash::Entity& SpawnDirectionLight(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 position, 
	DirectX::SimpleMath::Vector3 direction,
	DirectX::SimpleMath::Vector3 color)
{
	auto& light = game.GetEntities().Create();
	auto& light_thransform = light.Add<engine::TransformComponent>();
	light_thransform.SetPosition(position);

	auto& axis = engine::DebugUtils::CreateAxis(game, 1);
	axis.Get<engine::TransformComponent>().SetParent(light);

	auto& sphere = engine::DebugUtils::CreateSphere(game, 1, { 1.0f, 1.0f, 1.0f, 0.0f });
	sphere.Get<engine::TransformComponent>().SetParent(light);

	auto& line = engine::DebugUtils::CreateLine(game, position, position + direction, { 1.0f, 1.0f, 0.0f, 0.0f });

	auto& direction_light = light.Add<DirectionLightComponent>();
	direction_light.light_direction = direction;
	direction_light.light_color = color;
	return light;
}

ash::Entity& SpawnPointLight(
	engine::Game& game,
	DirectX::SimpleMath::Vector3 position,
	DirectX::SimpleMath::Vector3 color,
	float fade_radius)
{
	auto& light = game.GetEntities().Create();
	auto& light_transform = AddTransform(light, position);
	AttachModel(light, icosphere_model);
	light_transform.SetScale({ fade_radius, fade_radius, fade_radius });

	auto& point_light = light.Add<PointLightComponent>();
	point_light.light_color = color;
	point_light.fade_radius = fade_radius;

	auto& sphere = engine::DebugUtils::CreateSphere(game, 1, { 1.0f, 1.0f, 1.0f, 0.0f });
	sphere.Get<engine::TransformComponent>().SetParent(light);

	return light;
}

ash::Entity& SpawnTile(
	engine::Game& game, 
	DirectX::SimpleMath::Vector3 position = { 0.0f, 0.0f, 0.0f },
	DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f })
{
	auto& tile = game.GetEntities().Create();
	AddTransform(tile, position, { 0.0f, 0.0f, 0.0f }, scale);
	AttachModel(tile, tile_model);
	AttachMaterial(tile, { 0.1f, 10.0f, 0.1f });
	AttachDnsMaps(tile, tile_diffuse, tile_normal, tile_specular);
	return tile;
}


ash::Entity& SpawnPlane(engine::Game& game, DirectX::SimpleMath::Vector3 position) {
	using namespace DirectX::SimpleMath;
	auto& plane = game.GetEntities().Create();
	AddTransform(plane, position);
	constexpr int ratio = 7;
	constexpr int half_ratio = ratio / 2;
	constexpr float tile_size = 2;
	
	for (int i = 0; i < ratio; ++i) {
		for (int j = 0; j < ratio; ++j) {
			const float local_x = tile_size * (i - half_ratio) * 2;
			const float local_z = tile_size * (j - half_ratio) * 2;
			auto& tile = SpawnTile(game, {}, { tile_size, 1.0f, tile_size });
			auto& tile_transform = tile.Get<engine::TransformComponent>();
			tile_transform.SetParent(plane);
			tile_transform.SetLocalPosition(Vector3{ local_x, 0, local_z });
		}		
	}

	return plane;
}

std::error_code LoadModels() {
	using namespace engine;
	std::error_code ec;

	ec = Model::LoadObj("Content/Primitives/Icosphere.obj", &icosphere_model);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = Model::LoadObj("Content/Apricot/Apricot.obj", &apricot_model);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = Model::LoadObj("Content/Pear/Pear.obj", &pear_model);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = Model::LoadObj("Content/Statuette/Statuette.obj", &statuette_model);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = Model::LoadObj("Content/Apple/Apple.obj", &apple_model);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = Model::LoadObj("Content/Plane/Plane.obj", &tile_model);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	return ec;
}

std::error_code LoadDiffuse() {
	using namespace engine;
	std::error_code ec;

	ec = TextureLoader::LoadWicSrgb(L"Content/Apricot/Apricot_Diffuse.png", &apricot_diffuse);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWicSrgb(L"Content/Pear/Pear_Diffuse.jpg", &pear_diffuse);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWicSrgb(L"Content/Statuette/Statuette_Diffuse.png", &statuette_diffuse);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWicSrgb(L"Content/Apple/Apple_Diffuse.jpg", &apple_diffuse);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWicSrgb(L"Content/Plane/Plane_Diffuse.jpg", &tile_diffuse);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	return ec;
}

std::error_code LoadNormal() {
	using namespace engine;
	std::error_code ec;

	ec = TextureLoader::LoadWic(L"Content/Apricot/Apricot_Normal.png", &apricot_normal);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWic(L"Content/Pear/Pear_Normal.jpg", &pear_normal);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWic(L"Content/Statuette/Statuette_Normal.png", &statuette_normal);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWic(L"Content/Apple/Apple_Normal.jpg", &apple_normal);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWic(L"Content/Plane/Plane_Normal.png", &tile_normal);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	return ec;
}

std::error_code LoadSpecular() {
	using namespace engine;
	std::error_code ec;

	ec = TextureLoader::LoadWic(L"Content/Apricot/Apricot_Specular.png", &apricot_specular);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWic(L"Content/Pear/Pear_Specular.jpg", &pear_specular);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWic(L"Content/Statuette/Statuette_Specular.png", &statuette_specular);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	ec = TextureLoader::LoadWic(L"Content/Apple/Apple_Specular.jpg", &apple_specular);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	}

	// Use apple specular just because...
	ec = TextureLoader::LoadWic(L"Content/Apple/Apple_Specular.jpg", &tile_specular);
	if (ec != CoreErrors::kSuccess) {
		return ec;
	} 

	return ec;
}

void Release() {	
	apricot_diffuse.Release();
	pear_diffuse.Release();
	statuette_diffuse.Release();
	apple_diffuse.Release();
	tile_diffuse.Release();

	apricot_normal.Release();
	pear_normal.Release();
	statuette_normal.Release();
	apple_normal.Release();
	tile_normal.Release();

	apricot_specular.Release();
	pear_specular.Release();
	statuette_specular.Release();
	apple_specular.Release();
	tile_specular.Release();
}

int main() {
	if (HRESULT result = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED); FAILED(result)) {
		return result;
	}

	std::error_code ec;

	if (ec = LoadModels(); ec) {
		return ec.value();
	}
	if (ec = LoadDiffuse(); ec) {
		return ec.value();
	}
	if (ec = LoadNormal(); ec) {
		return ec.value();
	}
	if (ec = LoadSpecular(); ec) {
		return ec.value();
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
	
	auto& camera = SpawnCamera(game);
	auto& katamari = SpawnKatamari(game, camera);
	auto& grass = SpawnPlane(game, { 0.0f, -0.5f, 0.0f });

	SpawnItems(game);	

	auto& ambient_light = SpawnAmbientLight(game, { 1.0f, 1.0f, 1.0f }, 0.01f);

	DirectX::SimpleMath::Vector3 light_position = { 15.0f, 6.0f, 0.0f };
	DirectX::SimpleMath::Vector3 light_direction = { -1.0f, -2.0f, 0.0f };
	light_direction.Normalize();
	auto& direction_light = SpawnDirectionLight(game, light_position, light_direction, { 0.2f, 0.0f, 0.2f });

	auto& point_light1 = SpawnPointLight(game, { 6.0f, 1.0f, 6.0f }, { 0.6f, 0.6f, 0.6f }, 4);
	auto& point_light2 = SpawnPointLight(game, { -6.0f, 1.0f, -6.0f }, { 0.6f, 0.6f, 0.6f }, 4);

	game.Run();
	Release();
	return 0;
}