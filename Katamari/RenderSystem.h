#pragma once

#include <SimpleMath.h>
#include <d3d11.h>

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
	struct ConstData {
		DirectX::SimpleMath::Matrix mat;
	};

	RenderSystem()
		: shader_()
		, sampler_(D3D11_FILTER_MIN_MAG_MIP_LINEAR, 0)
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		using namespace graph;
		shader_.Init(&GetRenderer(), LayoutDescriptor::kPosition3Texture2, L"../Shaders/MaterialShader.hlsl");
		sampler_.Init(&GetRenderer());

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

			ConstantBuffer constant_buffer(sizeof(ConstData));
			constant_buffer.Init(&GetRenderer());

			Texture texture;
			texture.Init(&GetRenderer(), model_component.texture);

			entity.Add<RenderComponent>([&] {
				return new RenderComponent(model_buffers, constant_buffer, texture);
			});
		}		
	}

	void Update(float dt) override {
		auto camera_it = GetIterator<engine::CameraComponent>();
		if (!camera_it.HasCurrent()) return;

		ecs::Entity& camera = camera_it.Get();
		engine::CameraComponent& camera_component = camera.Get<engine::CameraComponent>();

		using namespace engine;
		for (auto it = GetIterator<RenderComponent, TransformComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& model = it.Get();
			RenderComponent& render_component = model.Get<RenderComponent>();
			TransformComponent& transform_component = model.Get<TransformComponent>();

			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
			DirectX::SimpleMath::Matrix camera_matrix = camera_component.GetCameraMatrix();
			DirectX::SimpleMath::Matrix transform_matrix = model_matrix * camera_matrix;
			ConstData data{ transform_matrix.Transpose() };

			render_component.constant_buffer.Update(&data);
		}
	}

	void Render() override {
		shader_.SetShader();
		sampler_.SetSampler();
		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		using namespace engine;
		for (auto it = GetIterator<RenderComponent>(); it.HasCurrent(); it.Next()) {
			ecs::Entity& model = it.Get();
			RenderComponent& render_component = model.Get<RenderComponent>();
			render_component.constant_buffer.SetBuffer();
			render_component.texture.SetTexture(0);

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
};