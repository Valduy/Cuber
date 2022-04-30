#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "../GraphicEngine/Renderer.h"

class GBuffer {
public:
	GBuffer()
		: renderer_(nullptr)
	{}

	HRESULT Init(graph::Renderer* renderer, UINT width, UINT height) {
		renderer_ = renderer;
		return Resize(width, height);
	}

	HRESULT Resize(UINT width, UINT height) {
		HRESULT result = S_OK;

		const D3D11_TEXTURE2D_DESC r8g8b8a8_desc = CreateTextureDescriptor(DXGI_FORMAT_R8G8B8A8_UNORM);
		const D3D11_TEXTURE2D_DESC r32g32b32a32_desc = CreateTextureDescriptor(DXGI_FORMAT_R32G32B32A32_FLOAT);

		if (CreateTexture2D(r8g8b8a8_desc, diffuse_texture_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateTexture2D(r32g32b32a32_desc, normal_texture_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateTexture2D(r32g32b32a32_desc, position_texture_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateTexture2D(r32g32b32a32_desc, accumulation_texture_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}

		if (CreateShaderResourceView(diffuse_texture_.Get(), diffuse_srv_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateShaderResourceView(normal_texture_.Get(), normal_srv_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateShaderResourceView(position_texture_.Get(), position_srv_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateShaderResourceView(accumulation_texture_.Get(), accumulation_srv_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}

		if (CreateRenderTargetView(diffuse_texture_.Get(), diffuse_rtv_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateRenderTargetView(normal_texture_.Get(), normal_rtv_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateRenderTargetView(position_texture_.Get(), position_rtv_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}
		if (CreateRenderTargetView(accumulation_texture_.Get(), accumulation_rtv_.ReleaseAndGetAddressOf()); FAILED(result)) {
			return result;
		}

		return result;
	}

private:
	graph::Renderer* renderer_;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> diffuse_texture_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> normal_texture_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> position_texture_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> accumulation_texture_;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuse_srv_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal_srv_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> position_srv_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> accumulation_srv_;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> diffuse_rtv_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> normal_rtv_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> position_rtv_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> accumulation_rtv_;

	D3D11_TEXTURE2D_DESC CreateTextureDescriptor(DXGI_FORMAT format) const {
		return {
			renderer_->GetWindow().GetWidth(),
			renderer_->GetWindow().GetHeight(),
			1,
			1,
			format,
			DXGI_SAMPLE_DESC{ 1, 0 },
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
		};
	}

	HRESULT CreateTexture2D(
		const D3D11_TEXTURE2D_DESC& texture_desc, 
		ID3D11Texture2D** texture) const
	{
		return renderer_->GetDevice().CreateTexture2D(
			&texture_desc, 
			nullptr, 
			texture);
	}

	HRESULT CreateShaderResourceView(
		ID3D11Texture2D* texture, 
		ID3D11ShaderResourceView** shader_resource_view) const
	{
		return renderer_->GetDevice().CreateShaderResourceView(
			texture, 
			nullptr, 
			shader_resource_view);
	}

	HRESULT CreateRenderTargetView(
		ID3D11Texture2D* texture, 
		ID3D11RenderTargetView** render_target_view) const
	{
		return renderer_->GetDevice().CreateRenderTargetView(
			texture,
			nullptr,
			render_target_view);
	}
};
