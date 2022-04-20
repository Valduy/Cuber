#pragma once

#include "DirectionLightComponent.h"
#include "RenderComponent.h"
#include "ShadowComponent.h"
#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/Vertex.h"
#include "../GraphicEngine/Shader.h"

namespace engine {
	class TransformComponent;
}

class ShadowMapRenderSystem : public engine::Game::SystemBase {
public:
	struct TransformData {
		DirectX::SimpleMath::Matrix world_view_proj;
	};

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		using namespace graph;
		shader_.Init(&GetRenderer(), LayoutDescriptor::kPosition3Normal3Texture2, L"Shaders/ShadowMapShader.hlsl");

		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();
		light_component.shadow_map.Init(&GetRenderer(), 1024, 1024);

		using namespace engine;
		auto it = GetIterator<RenderComponent, TransformComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& model = it.Get();

			ConstantBuffer transform_buffer(sizeof(TransformData));
			transform_buffer.Init(&GetRenderer());

			model.Add<ShadowComponent>([&] {
				return new ShadowComponent(transform_buffer);
			});
		}
	}

	void Update(float dt) override {
		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();

		using namespace engine;
		auto it = GetIterator<RenderComponent, TransformComponent, ShadowComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& model = it.Get();
			auto& render_component = model.Get<RenderComponent>();
			auto& transform_component = model.Get<TransformComponent>();
			auto& shadow_component = model.Get<ShadowComponent>();

			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
			DirectX::SimpleMath::Matrix light_matrix = light_component.GetLightMatrix();
			DirectX::SimpleMath::Matrix world_view_proj_matrix = model_matrix * light_matrix;
			TransformData transform_data{
				world_view_proj_matrix.Transpose(),
			};

			shadow_component.transform_buffer.Update(&transform_data);
		}
	}

	void Render() override {
		shader_.SetShader();

		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();
		light_component.shadow_map.SetRenderTarget();

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		using namespace engine;
		auto it = GetIterator<RenderComponent, ShadowComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& model = it.Get();
			auto& render_component = model.Get<RenderComponent>();
			auto& shadow_component = model.Get<ShadowComponent>();
			shadow_component.transform_buffer.VSSetBuffer();
			render_component.texture.SetTexture();

			for (MeshBuffers& mesh_buffers : render_component.model_buffers) {
				mesh_buffers.vertex_buffers.SetBuffer(sizeof(Vertex));
				mesh_buffers.index_buffers.SetBuffer();
				GetRenderer().GetContext().DrawIndexed(
					mesh_buffers.index_buffers.GetSize(), 0, 0);
			}
		}
		
		GetGame().GetRenderer().SetDefaultRenderTarget();
	}

private:
	graph::Shader shader_;	
};
