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
		using namespace graph;
		using namespace engine;

		SystemBase::Init(game);
		
		shader_.Init(
			&GetRenderer(), 
			LayoutDescriptor::kPosition3Normal3Binormal3Tangent3Texture2, 
			L"Shaders/ShadowMapShader.hlsl");

		auto it = Filter<DirectionLightComponent>().GetIterator();
		if (!it.HasCurrent()) return;

		auto& [entity, direction_light_component] = it.Get();
		direction_light_component.shadow_map.Init(&GetRenderer(), 2048, 2048);

		for (auto& [entity, model_component, transform_component] : Filter<ModelComponent, TransformComponent>()) {
			ConstantBuffer transform_buffer;
			transform_buffer.Init(&GetRenderer(), sizeof(TransformData));

			entity.Add<ShadowComponent>([&] {
				return new ShadowComponent(transform_buffer);
			});
		}
	}

	void Update(float dt) override {
		using namespace engine;

		DirectX::SimpleMath::Matrix light_matrix;
		if (!TryGetLightMatrix(&light_matrix)) return;

		for (auto& [entity, transform_component, shadow_component] : Filter<TransformComponent, ModelComponent>()) {
			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
			DirectX::SimpleMath::Matrix world_view_proj_matrix = model_matrix * light_matrix;
			TransformData transform_data{
				world_view_proj_matrix.Transpose(),
			};

			shadow_component.transform_buffer.Update(&transform_data);
		}
	}

	void Render() override {
		using namespace engine;

		shader_.SetShader();

		auto it = Filter<DirectionLightComponent>().GetIterator();
		if (!it.HasCurrent()) return;

		auto& [entity, direction_light_component] = it.Get();
		direction_light_component.shadow_map.SetRenderTarget();
		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		for (auto& node : Filter<ModelComponent, DnsMapsComponent, ShadowComponent>()) {
			auto& [entity, model_component, dns_maps_component, shadow_component] = node;
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
		auto it = Filter<DirectionLightComponent>().GetIterator();
		if (!it.HasCurrent()) return false;

		auto& [entity, direction_light_component] = it.Get();
		*light_matrix = direction_light_component.GetLightMatrix();
		return true;
	}
};
