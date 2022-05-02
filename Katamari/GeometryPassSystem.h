#pragma once

#include "GBuffer.h"
#include "../GraphicEngine/Shader.h"
#include "../Engine/Game.h"
#include "../Engine/TextureLoader.h"

class GeometryPassSystem : public engine::Game::SystemBase {
public:
	GeometryPassSystem()
		: g_buffer_()
		, shader_()
		, sampler_(D3D11_FILTER_MIN_MAG_MIP_LINEAR, 0)
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		CreateRasterizerState();
		CreateBlendState();
		CreateDepthStencilState();

		g_buffer_.Init(
			&GetRenderer(), 
			GetRenderer().GetWindow().GetWidth(), 
			GetRenderer().GetWindow().GetHeight());

		shader_.Init(
			&GetRenderer(), 
			graph::LayoutDescriptor::kPosition3Normal3Binormal3Tangent3Texture2,
			L"Shaders/GeometryPassShader.hlsl");

		t_shader_.Init(
			&GetRenderer(),
			graph::LayoutDescriptor::kEmpty,
			L"Shaders/FillShader.hlsl");
				
		//engine::TextureLoader::LoadWic(L"../Content/Pumpkin_Diffuse.jpg", &image);
		//t_texture_.Init(&GetRenderer(), image);

		sampler_.Init(&GetRenderer());
	}

	void Update(float dt) override {

	}

	void Render() override {
		OpaquePass();
		
		GetRenderer().SetDefaultRenderTarget();
		t_shader_.SetShader();		
		ID3D11ShaderResourceView* srv = &g_buffer_.GetDiffuseShaderResourceView();
		//ID3D11ShaderResourceView* srv = &g_buffer_.GetNormalShaderResourceView();
		//ID3D11ShaderResourceView* srv = &g_buffer_.GetPositionShaderResourceView();
		GetRenderer().GetContext().PSSetShaderResources(0, 1, &srv);
		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		GetRenderer().GetContext().IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
		GetRenderer().GetContext().Draw(4, 0);
	}

private:
	GBuffer g_buffer_;
	graph::Shader shader_;
	////DirectX::ScratchImage image;
	////graph::Texture t_texture_;
	graph::Shader t_shader_;
	graph::Sampler sampler_;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> cull_back_rs_;
	Microsoft::WRL::ComPtr<ID3D11BlendState> opaque_bs_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> opaque_dss_;

	void OpaquePass() {
		GetRenderer().GetContext().ClearDepthStencilView(
			&GetRenderer().GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		GetRenderer().GetContext().RSSetState(cull_back_rs_.Get());
		GetRenderer().GetContext().OMSetBlendState(opaque_bs_.Get(), nullptr, 0xffffffff);
		GetRenderer().GetContext().OMSetDepthStencilState(opaque_dss_.Get(), 0);

		shader_.SetShader();
		sampler_.SetSampler();
		g_buffer_.SetGeometryPassTargets();
		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		using namespace engine;
		auto it = GetIterator<RenderComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& model = it.Get();
			auto& render_component = model.Get<RenderComponent>();
			render_component.transform_buffer.VSSetBuffer(0);
			render_component.diffuse.SetTexture(0);
			render_component.normal.SetTexture(1);
			render_component.specular.SetTexture(2);

			for (MeshBuffers& mesh_buffers : render_component.model_buffers) {
				mesh_buffers.vertex_buffer.SetBuffer(sizeof(Vertex));
				mesh_buffers.index_buffer.SetBuffer();
				GetRenderer().GetContext().DrawIndexed(
					mesh_buffers.index_buffer.GetSize(), 0, 0);
			}
		}
	}

	HRESULT CreateRasterizerState() {
		D3D11_RASTERIZER_DESC raster_desc{};
		raster_desc.CullMode = D3D11_CULL_BACK;
		raster_desc.FillMode = D3D11_FILL_SOLID;

		return GetRenderer().GetDevice().CreateRasterizerState(&raster_desc, cull_back_rs_.GetAddressOf());
	}

	HRESULT CreateBlendState() {
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

	HRESULT CreateDepthStencilState() {
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		return GetRenderer().GetDevice().CreateDepthStencilState(&depth_stencil_desc, opaque_dss_.GetAddressOf());
	}
};