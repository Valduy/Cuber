#pragma once

#include "AmbientLightComponent.h"
#include "GBuffer.h"
#include "PointLightComponent.h"
#include "../GraphicEngine/Shader.h"
#include "../Engine/Game.h"

class DefferedRenderSystem : public engine::Game::SystemBase {
public:
	DefferedRenderSystem()
		: sampler_()
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		CreateCullBackRasterizerState();
		CreateCullFrontRasterizerState();
		CreateOpaqueBlendState();
		CreateLightBlendState();
		CreateOpaqueDepthStencilState();
		CreateLightLessDepthStencilState();
		CreateLightGreaterDepthStencilState();
		CreateNoDepthDepthStencilState();

		g_buffer_.Init(
			&GetRenderer(), 
			GetRenderer().GetWindow().GetWidth(), 
			GetRenderer().GetWindow().GetHeight());

		opaque_shader_.Init(
			&GetRenderer(), 
			graph::LayoutDescriptor::kPosition3Normal3Binormal3Tangent3Texture2,
			L"Shaders/GeometryPassShader.hlsl");

		ambient_light_shader_.Init(
			&GetRenderer(),
			graph::LayoutDescriptor::kEmpty,
			L"Shaders/AmbientLightPassShader.hlsl");

		direction_light_shader_.Init(
			&GetRenderer(),
			graph::LayoutDescriptor::kEmpty,
			L"Shaders/DirectionLightPassShader.hlsl");

		point_light_shader_.Init(
			&GetRenderer(),
			graph::LayoutDescriptor::kPosition3Normal3Binormal3Tangent3Texture2,
			L"Shaders/PointLightPassShader.hlsl");

		tone_pass_shader_.Init(
			&GetRenderer(),
			graph::LayoutDescriptor::kEmpty,
			L"Shaders/TonePassShader.hlsl");
				
		//engine::TextureLoader::LoadWic(L"../Content/Pumpkin_Diffuse.jpg", &image);
		//t_texture_.Init(&GetRenderer(), image);

		sampler_.Init(
			&GetRenderer(),
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_COMPARISON_NEVER,
			1);
	}

	void Update(float dt) override {

	}

	void Render() override {
		OpaquePass();
		LightPass();
		TonePass();
	}

private:
	GBuffer g_buffer_;
	graph::Shader opaque_shader_;
	graph::Shader ambient_light_shader_;
	graph::Shader direction_light_shader_;	
	graph::Shader point_light_shader_;	
	graph::Shader tone_pass_shader_;
	graph::Sampler sampler_;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> cull_back_rs_;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> cull_front_rs_;

	Microsoft::WRL::ComPtr<ID3D11BlendState> opaque_bs_;
	Microsoft::WRL::ComPtr<ID3D11BlendState> light_bs_;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> opaque_dss_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> light_less_dss_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> light_greater_dss_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> no_depth_dss_;

	void OpaquePass() {
		using namespace engine;

		GetRenderer().GetContext().RSSetState(cull_back_rs_.Get());
		GetRenderer().GetContext().OMSetBlendState(opaque_bs_.Get(), nullptr, 0xffffffff);
		GetRenderer().GetContext().OMSetDepthStencilState(opaque_dss_.Get(), 0);

		opaque_shader_.SetShader();
		sampler_.SetSampler();
		g_buffer_.SetGeometryPassTargets();
		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		for (auto& [entity, model_component, dns_maps_component] : Filter<ModelComponent, DnsMapsComponent>()) {
			model_component.transform_buffer.VSSetBuffer(0);
			dns_maps_component.diffuse_texture.SetTexture(0);
			dns_maps_component.normal_texture.SetTexture(1);
			dns_maps_component.specular_texture.SetTexture(2);

			for (MeshBuffers& mesh_buffers : model_component.model_buffers) {
				mesh_buffers.vertex_buffer.SetBuffer(sizeof(Vertex));
				mesh_buffers.index_buffer.SetBuffer();
				GetRenderer().GetContext().DrawIndexed(
					mesh_buffers.index_buffer.GetSize(), 0, 0);
			}
		}
	}

	void LightPass() {
		GetRenderer().GetContext().OMSetBlendState(light_bs_.Get(), nullptr, 0xffffffff);
		g_buffer_.SetLightPassTargets();

		RenderAmbientLights();
		RenderDirectionLights();
		RenderPointLights();
	}

	void TonePass() {
		GetRenderer().SetDefaultRenderTarget();
		GetRenderer().GetContext().RSSetState(cull_back_rs_.Get());
		GetRenderer().GetContext().OMSetBlendState(opaque_bs_.Get(), nullptr, 0xffffffff);
		GetRenderer().GetContext().OMSetDepthStencilState(no_depth_dss_.Get(), 0);

		tone_pass_shader_.SetShader();

		//ID3D11ShaderResourceView* srv = &g_buffer_.GetDiffuseShaderResourceView();
		//ID3D11ShaderResourceView* srv = &g_buffer_.GetNormalShaderResourceView();
		//ID3D11ShaderResourceView* srv = &g_buffer_.GetPositionShaderResourceView();
		ID3D11ShaderResourceView* srv = &g_buffer_.GetAccumulationShaderResourceView();
		GetRenderer().GetContext().PSSetShaderResources(0, 1, &srv);

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		GetRenderer().GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
		GetRenderer().GetContext().Draw(4, 0);

		// Just because I want to return default depth stencil state...
		GetRenderer().SetDefaultRenderTarget();
	}

	void RenderAmbientLights() {
		GetRenderer().GetContext().RSSetState(cull_back_rs_.Get());
		GetRenderer().GetContext().OMSetDepthStencilState(light_less_dss_.Get(), 0);

		ambient_light_shader_.SetShader();
		g_buffer_.SetResources();

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		GetRenderer().GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

		for (auto& [entity, ambient_light_component] : Filter<AmbientLightComponent>()) {
			ambient_light_component.light_data_buffer.PSSetBuffer(0);
			GetRenderer().GetContext().Draw(4, 0);
		}
	}

	void RenderDirectionLights() {
		GetRenderer().GetContext().RSSetState(cull_back_rs_.Get());
		GetRenderer().GetContext().OMSetDepthStencilState(light_less_dss_.Get(), 0);

		direction_light_shader_.SetShader();		
		g_buffer_.SetResources();

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		GetRenderer().GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);

		for (auto& [entity, direction_light_component] : Filter<DirectionLightComponent>()) {
			direction_light_component.light_data_buffer.PSSetBuffer(0);
			GetRenderer().GetContext().Draw(4, 0);
		}
	}

	void RenderPointLights() {
		GetRenderer().GetContext().RSSetState(cull_front_rs_.Get());
		GetRenderer().GetContext().OMSetDepthStencilState(light_greater_dss_.Get(), 0);

		point_light_shader_.SetShader();
		g_buffer_.SetResources();

		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto& [entity, model_component, point_light_component] : Filter<ModelComponent, PointLightComponent>()) {
			model_component.transform_buffer.VSSetBuffer(0);
			point_light_component.light_data_buffer.PSSetBuffer(1);

			for (MeshBuffers& mesh_buffers : model_component.model_buffers) {
				mesh_buffers.vertex_buffer.SetBuffer(sizeof(engine::Vertex));
				mesh_buffers.index_buffer.SetBuffer();
				GetRenderer().GetContext().DrawIndexed(
					mesh_buffers.index_buffer.GetSize(), 0, 0);
			}
		}
	}

	HRESULT CreateCullBackRasterizerState() {
		D3D11_RASTERIZER_DESC raster_desc{};
		raster_desc.CullMode = D3D11_CULL_BACK;
		raster_desc.FillMode = D3D11_FILL_SOLID;

		return GetRenderer().GetDevice().CreateRasterizerState(&raster_desc, cull_back_rs_.GetAddressOf());
	}

	HRESULT CreateCullFrontRasterizerState() {
		D3D11_RASTERIZER_DESC raster_desc{};
		raster_desc.CullMode = D3D11_CULL_FRONT;
		raster_desc.FillMode = D3D11_FILL_SOLID;

		return GetRenderer().GetDevice().CreateRasterizerState(&raster_desc, cull_front_rs_.GetAddressOf());
	}

	HRESULT CreateOpaqueBlendState() {
		D3D11_BLEND_DESC blend_state_desc { false, false };
		blend_state_desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		blend_state_desc.RenderTarget[0].BlendEnable = true;
		blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		return GetRenderer().GetDevice().CreateBlendState(&blend_state_desc, opaque_bs_.GetAddressOf());
	}

	HRESULT CreateLightBlendState() {
		D3D11_BLEND_DESC blend_state_desc{ false, false };
		blend_state_desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;
		blend_state_desc.RenderTarget[0].BlendEnable = true;
		blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

		return GetRenderer().GetDevice().CreateBlendState(&blend_state_desc, light_bs_.GetAddressOf());
	}

	HRESULT CreateOpaqueDepthStencilState() {
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		return GetRenderer().GetDevice().CreateDepthStencilState(&depth_stencil_desc, opaque_dss_.GetAddressOf());
	}

	HRESULT CreateLightLessDepthStencilState() {
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		
		return GetRenderer().GetDevice().CreateDepthStencilState(&depth_stencil_desc, light_less_dss_.GetAddressOf());
	}

	HRESULT CreateLightGreaterDepthStencilState() {
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_GREATER;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		return GetRenderer().GetDevice().CreateDepthStencilState(&depth_stencil_desc, light_greater_dss_.GetAddressOf());
	}

	HRESULT CreateNoDepthDepthStencilState() {
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = false;

		return GetRenderer().GetDevice().CreateDepthStencilState(&depth_stencil_desc, no_depth_dss_.GetAddressOf());
	}
};