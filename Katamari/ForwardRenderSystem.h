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
		engine::Game::SystemBase::Init(game);

		using namespace graph;
		using namespace engine;

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
				
		for (auto it = GetIterator<TransformComponent, ModelComponent>(); it.HasCurrent(); it.Next()) {
			auto& entity = it.Get();
			auto& forward_render_component = entity.Add<ForwardRenderComponent>();
			forward_render_component.light_transform_buffer.Init(&GetRenderer(), sizeof(LightTransformData));
		}
	}

	void Update(float dt) override {
		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();

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

		// Now I use only first direction light...
		if (!TrySetLightData()) return;

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		using namespace engine;
		const auto sign = ecs::Signer::GetSignature<
			ModelComponent,
			MaterialComponent,
			DnsMapsComponent,
			ForwardRenderComponent>();

		for (auto it = GetIterator(sign); it.HasCurrent(); it.Next()) {
			auto& model = it.Get();
			auto& model_component = model.Get<ModelComponent>();
			auto& material_component = model.Get<MaterialComponent>();
			auto& dns_maps_component = model.Get<DnsMapsComponent>();
			auto& forward_render_component = model.Get<ForwardRenderComponent>();

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
		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return false;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();
		light_component.light_data_buffer.PSSetBuffer(3);
		light_component.shadow_map.SetTexture(1);
		return true;
	}
};