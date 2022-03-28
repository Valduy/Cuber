#pragma once

#include <SimpleMath.h>
#include <d3d11.h>

#include "DirectionLightComponent.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/CameraComponent.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/Sampler.h"
#include "../GraphicEngine/LayoutDescriptor.h"
#include "RenderComponent.h"
#include "ModelComponent.h"

class RenderSystem : public engine::Game::SystemBase {
public:
	struct TransformData {
		DirectX::SimpleMath::Matrix world;
		DirectX::SimpleMath::Matrix world_view_proj;
		DirectX::SimpleMath::Matrix inverse_transpose_world;		
	};

	struct MaterialData {
		float ambient;
		float shininess;
		float specular;
		float dummy;
	};

	struct LightData {
		DirectX::SimpleMath::Vector3 view_position;
		float dummy0;
		DirectX::SimpleMath::Vector3 light_direction;
		float dummy1;
		DirectX::SimpleMath::Vector3 light_color;
		float dummy2;
	};

	RenderSystem()
		: shader_()
		, sampler_(D3D11_FILTER_MIN_MAG_MIP_LINEAR, 0)
		, light_buffer(sizeof(LightData))
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		using namespace graph;
		shader_.Init(&GetRenderer(), LayoutDescriptor::kPosition3Normal3Texture2, L"../Shaders/MaterialShader.hlsl");
		sampler_.Init(&GetRenderer());
		light_buffer.Init(&GetRenderer());

		using namespace engine;
		for (auto it = GetIterator<TransformComponent, ModelComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			ModelComponent& model_component = entity.Get<ModelComponent>();
			std::vector<MeshBuffers> model_buffers;

			for (const Mesh& mesh : model_component.model.GetMeshes()) {
				VertexBuffer vb(mesh.vertices.data(), sizeof(Vertex) * mesh.vertices.size());
				vb.Init(&GetRenderer());

				IndexBuffer ib(mesh.indices.data(), mesh.indices.size());
				ib.Init(&GetRenderer());
				model_buffers.push_back({ vb, ib });
			}

			ConstantBuffer transform_buffer(sizeof(TransformData));
			transform_buffer.Init(&GetRenderer());

			ConstantBuffer material_buffer(sizeof(MaterialData));
			material_buffer.Init(&GetRenderer());
			MaterialData material_data{
				model_component.material.ambient,
				model_component.material.shininess,
				model_component.material.specular
			};
			material_buffer.Update(&material_data);

			Texture texture;
			texture.Init(&GetRenderer(), model_component.texture);

			entity.Add<RenderComponent>([&] {
				return new RenderComponent(
					model_buffers, 
					transform_buffer, 
					material_buffer,
					texture);
			});
		}		
	}

	void Update(float dt) override {
		auto camera_it = GetIterator<engine::CameraComponent>();
		if (!camera_it.HasCurrent()) return;

		ecs::Entity& camera = camera_it.Get();
		engine::CameraComponent& camera_component = camera.Get<engine::CameraComponent>();

		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		ecs::Entity& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();
		
		LightData light_data{};
		light_data.view_position = camera_component.position;
		light_data.light_direction = light_component.light_direction;
		light_data.light_color = light_component.light_color;
		light_buffer.Update(&light_data);

		using namespace engine;
		for (auto it = GetIterator<RenderComponent, TransformComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& model = it.Get();
			RenderComponent& render_component = model.Get<RenderComponent>();
			TransformComponent& transform_component = model.Get<TransformComponent>();

			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
			DirectX::SimpleMath::Matrix camera_matrix = camera_component.GetCameraMatrix();
			DirectX::SimpleMath::Matrix world_view_proj_matrix = model_matrix * camera_matrix;
			TransformData transform_data{
				model_matrix.Transpose(),
				world_view_proj_matrix.Transpose(),
				model_matrix.Transpose().Invert().Transpose(),
			};

			render_component.transform_buffer.Update(&transform_data);
		}
	}

	void Render() override {
		shader_.SetShader();
		sampler_.SetSampler();
		light_buffer.PSSetBuffer(2);

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		using namespace engine;
		for (auto it = GetIterator<RenderComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& model = it.Get();
			RenderComponent& render_component = model.Get<RenderComponent>();
			render_component.transform_buffer.VSSetBuffer(0);
			render_component.material_buffer.PSSetBuffer(1);
			render_component.texture.SetTexture();

			for (MeshBuffers& mesh_buffers : render_component.model_buffers) {
				mesh_buffers.vertex_buffers.SetBuffer(sizeof(Vertex));
				mesh_buffers.index_buffers.SetBuffer();
				GetRenderer().GetContext().DrawIndexed(
					mesh_buffers.index_buffers.GetSize(), 0, 0);
			}			
		}
	}

private:
	graph::Shader shader_;
	graph::Sampler sampler_;
	graph::ConstantBuffer light_buffer;
};