#pragma once

#include <SimpleMath.h>
#include <d3d11.h>

#include "DirectionLightComponent.h"
#include "DnsMapsComponent.h"
#include "ForwardRenderComponent.h"
#include "ModelComponent.h"
#include "MaterialComponent.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/Sampler.h"
#include "../GraphicEngine/LayoutDescriptor.h"

class ForwardRenderSystem : public engine::Game::SystemBase {
public:
	struct LightTransformData {
		DirectX::SimpleMath::Matrix light_world_view_proj;
	};

	void Init(engine::Game& game) override {
		using namespace graph;
		using namespace engine;

		SystemBase::Init(game);
		
		shader_.Init(
			&GetRenderer(), 
			LayoutDescriptor::kPosition3Normal3Binormal3Tangent3Texture2,
			L"Shaders/ForwardShader.hlsl");

		sampler_.Init(
			&GetRenderer(),
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_COMPARISON_ALWAYS,
			0);

		for (auto& [entity, transform_component, model_component] : Filter<TransformComponent, ModelComponent>()) {
			auto& forward_render_component = entity.Add<ForwardRenderComponent>();
			forward_render_component.light_transform_buffer.Init(&GetRenderer(), sizeof(LightTransformData));
		}
	}

	void Update(float dt) override {
		using namespace engine;

		auto it = Filter<DirectionLightComponent>().GetIterator();
		if (!it.HasCurrent()) return;

		auto& [entity, direction_light_component] = it.Get();
		
		for (auto& node : Filter<TransformComponent, ForwardRenderComponent>()) {
			auto& [entity, transform_component, forward_render_component] = node;
			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
			DirectX::SimpleMath::Matrix light_matrix = direction_light_component.GetLightMatrix();
			DirectX::SimpleMath::Matrix light_world_view_proj_matrix = model_matrix * light_matrix;

			LightTransformData light_transform_data {
				light_world_view_proj_matrix.Transpose()
			};
			forward_render_component.light_transform_buffer.Update(&light_transform_data);
		}
	}

	void Render() override {
		using namespace engine;

		shader_.SetShader();
		sampler_.SetSampler();

		// Now I use only first direction light...
		if (!TrySetLightData()) return;

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		for (auto& node : Filter<ModelComponent, MaterialComponent, DnsMapsComponent, ForwardRenderComponent>()) {
			auto& [entity, model_component, material_component, dns_maps_component, forward_render_component] = node;
			model_component.transform_buffer.VSSetBuffer(0);
			forward_render_component.light_transform_buffer.VSSetBuffer(1);
			material_component.material_buffer.PSSetBuffer(2);
			dns_maps_component.diffuse_texture.SetTexture(0);

			for (MeshBuffers& mesh_buffers : model_component.model_buffers) {
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

	bool TrySetLightData() {
		auto it = Filter<DirectionLightComponent>().GetIterator();
		if (!it.HasCurrent()) return false;

		auto& [entity, light_component] = it.Get();
		light_component.light_data_buffer.PSSetBuffer(3);
		light_component.shadow_map.SetTexture(1);
		return true;
	}
};