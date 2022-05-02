#pragma once

#include "DirectionLightComponent.h"
#include "ShadowComponent.h"
#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/Vertex.h"
#include "../GraphicEngine/Shader.h"

class ShadowMapRenderSystem : public engine::Game::SystemBase {
public:
	struct TransformData {
		DirectX::SimpleMath::Matrix world_view_proj;
	};

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		using namespace graph;
		shader_.Init(
			&GetRenderer(), 
			LayoutDescriptor::kPosition3Normal3Binormal3Tangent3Texture2, 
			L"Shaders/ShadowMapShader.hlsl");

		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();
		light_component.shadow_map.Init(&GetRenderer(), 2048, 2048);

		using namespace engine;
		auto it = GetIterator<ModelComponent, TransformComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& model = it.Get();

			ConstantBuffer transform_buffer;
			transform_buffer.Init(&GetRenderer(), sizeof(TransformData));

			model.Add<ShadowComponent>([&] {
				return new ShadowComponent(transform_buffer);
			});
		}
	}

	void Update(float dt) override {
		DirectX::SimpleMath::Matrix light_matrix;
		if (!TryGetLightMatrix(&light_matrix)) return;

		using namespace engine;
		auto it = GetIterator<TransformComponent, ShadowComponent>();
		for (; it.HasCurrent(); it.Next()) {
			auto& model = it.Get();
			auto& transform_component = model.Get<TransformComponent>();
			auto& shadow_component = model.Get<ShadowComponent>();

			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
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

		const auto sign = ecs::Signer::GetSignature<
			ModelComponent,
			DnsMapsComponent,
			ShadowComponent>();

		for (auto it = GetIterator(sign); it.HasCurrent(); it.Next()) {
			auto& model = it.Get();
			auto& model_component = model.Get<ModelComponent>();
			auto& dns_maps_component = model.Get<DnsMapsComponent>();
			auto& shadow_component = model.Get<ShadowComponent>();
			shadow_component.transform_buffer.VSSetBuffer();
			dns_maps_component.diffuse_texture.SetTexture();

			for (MeshBuffers& mesh_buffers : model_component.model_buffers) {
				mesh_buffers.vertex_buffer.SetBuffer(sizeof(Vertex));
				mesh_buffers.index_buffer.SetBuffer();
				GetRenderer().GetContext().DrawIndexed(
					mesh_buffers.index_buffer.GetSize(), 0, 0);
			}
		}
		
		GetGame().GetRenderer().SetDefaultRenderTarget();
	}

private:
	graph::Shader shader_;

	bool TryGetLightMatrix(DirectX::SimpleMath::Matrix* light_matrix) {
		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return false;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();
		*light_matrix = light_component.GetLightMatrix();
		return true;
	}
};
