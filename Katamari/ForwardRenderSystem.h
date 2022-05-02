#pragma once

#include <SimpleMath.h>
#include <d3d11.h>

#include "DirectionLightComponent.h"
#include "ForwardRenderComponent.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/CameraComponent.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/Sampler.h"
#include "../GraphicEngine/LayoutDescriptor.h"
#include "RenderComponent.h"
#include "ModelComponent.h"

class ForwardRenderSystem : public engine::Game::SystemBase {
public:
	struct LightTransformData {
		DirectX::SimpleMath::Matrix light_world_view_proj;
	};

	struct LightData {
		DirectX::SimpleMath::Vector3 view_position;
		float dummy0;
		DirectX::SimpleMath::Vector3 light_direction;
		float dummy1;
		DirectX::SimpleMath::Vector3 light_color;
		float dummy2;
	};

	ForwardRenderSystem()
		: shader_()
		, sampler_(D3D11_FILTER_MIN_MAG_MIP_LINEAR, 0)
		, light_buffer(sizeof(LightData))
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		using namespace graph;
		shader_.Init(
			&GetRenderer(), 
			LayoutDescriptor::kPosition3Normal3Binormal3Tangent3Texture2,
			L"Shaders/ForwardShader.hlsl");
		sampler_.Init(&GetRenderer());
		light_buffer.Init(&GetRenderer());

		using namespace engine;
		for (auto it = GetIterator<TransformComponent, ModelComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();

			ConstantBuffer light_transform_buffer(sizeof(LightTransformData));
			light_transform_buffer.Init(&GetRenderer());

			entity.Add<ForwardRenderComponent>([&] {
				return new ForwardRenderComponent(light_transform_buffer);
			});
		}
	}

	void Update(float dt) override {
		auto camera_it = GetIterator<engine::CameraComponent>();
		if (!camera_it.HasCurrent()) return;

		auto& camera = camera_it.Get();
		auto& camera_component = camera.Get<engine::CameraComponent>();

		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();
		
		LightData light_data{};
		light_data.view_position = camera_component.position;
		light_data.light_direction = light_component.light_direction;
		light_data.light_color = light_component.light_color;
		light_buffer.Update(&light_data);

		using namespace engine;
		auto it = GetIterator<TransformComponent, ForwardRenderComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& model = it.Get();
			auto& transform_component = model.Get<TransformComponent>();
			auto& forward_render_component = model.Get<ForwardRenderComponent>();

			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
			DirectX::SimpleMath::Matrix light_matrix = light_component.GetLightMatrix();
			DirectX::SimpleMath::Matrix light_world_view_proj_matrix = model_matrix * light_matrix;

			LightTransformData light_transform_data {
				light_world_view_proj_matrix.Transpose()
			};
			forward_render_component.light_transform_buffer.Update(&light_transform_data);
		}
	}

	void Render() override {
		shader_.SetShader();
		sampler_.SetSampler();
		light_buffer.PSSetBuffer(3);

		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();
		light_component.shadow_map.SetTexture(1);

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		using namespace engine;
		auto it = GetIterator<RenderComponent, ForwardRenderComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& model = it.Get();
			auto& render_component = model.Get<RenderComponent>();
			render_component.transform_buffer.VSSetBuffer(0);
			render_component.material_buffer.PSSetBuffer(2);
			render_component.diffuse.SetTexture(0);

			auto& forward_render_component = model.Get<ForwardRenderComponent>();
			forward_render_component.light_transform_buffer.VSSetBuffer(1);

			for (MeshBuffers& mesh_buffers : render_component.model_buffers) {
				mesh_buffers.vertex_buffer.SetBuffer(sizeof(Vertex));
				mesh_buffers.index_buffer.SetBuffer();
				GetRenderer().GetContext().DrawIndexed(
					mesh_buffers.index_buffer.GetSize(), 0, 0);
			}			
		}
	}

private:
	graph::Shader shader_;
	graph::Sampler sampler_;
	graph::ConstantBuffer light_buffer;

	std::vector<MeshBuffers> CreateMeshBuffers(const engine::Model& model) {
		std::vector<MeshBuffers> model_buffers;

		for (const engine::Mesh& mesh : model.GetMeshes()) {
			graph::VertexBuffer vb(mesh.vertices.data(), sizeof(engine::Vertex) * mesh.vertices.size());
			vb.Init(&GetRenderer());

			graph::IndexBuffer ib(mesh.indices.data(), mesh.indices.size());
			ib.Init(&GetRenderer());

			model_buffers.push_back({ vb, ib });
		}

		return model_buffers;
	}
};