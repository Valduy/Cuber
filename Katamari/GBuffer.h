#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <wrl/client.h>
#include "../GraphicEngine/Renderer.h"

class GBuffer {
public:
	ID3D11Texture2D& GetDiffuseTexture() const {
		assert(diffuse_texture_ != nullptr && "GBuffer isn't initialized.");
		return *diffuse_texture_.Get();
	}

	ID3D11Texture2D& GetNormalTexture() const {
		assert(normal_texture_ != nullptr && "GBuffer isn't initialized.");
		return *normal_texture_.Get();
	}

	ID3D11Texture2D& GetPositionTexture() const {
		assert(position_texture_ != nullptr && "GBuffer isn't initialized.");
		return *position_texture_.Get();
	}

	ID3D11Texture2D& GetAccumulationTexture() const {
		assert(accumulation_texture_ != nullptr && "GBuffer isn't initialized.");
		return *accumulation_texture_.Get();
	}

	ID3D11ShaderResourceView& GetDiffuseShaderResourceView() const {
		assert(diffuse_srv_ != nullptr && "GBuffer isn't initialized.");
		return *diffuse_srv_.Get();
	}

	ID3D11ShaderResourceView& GetNormalShaderResourceView() const {
		assert(normal_srv_ != nullptr && "GBuffer isn't initialized.");
		return *normal_srv_.Get();
	}

	ID3D11ShaderResourceView& GetPositionShaderResourceView() const {
		assert(position_srv_ != nullptr && "GBuffer isn't initialized.");
		return *position_srv_.Get();
	}

	ID3D11ShaderResourceView& GetAccumulationShaderResourceView() const {
		assert(accumulation_srv_ != nullptr && "GBuffer isn't initialized.");
		return *accumulation_srv_.Get();
	}

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

	void SetGeometryPassTargets() const {
		DirectX::SimpleMath::Color clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderer_->GetContext().ClearRenderTargetView(diffuse_rtv_.Get(), clear_color);
		renderer_->GetContext().ClearRenderTargetView(normal_rtv_.Get(), clear_color);
		renderer_->GetContext().ClearRenderTargetView(position_rtv_.Get(), clear_color);

		ID3D11RenderTargetView* views[] = {
			diffuse_rtv_.Get(),
			normal_rtv_.Get(),
			position_rtv_.Get(),
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
		};
		renderer_->GetContext().OMSetRenderTargets(8, views, &renderer_->GetDepthStencilView());
	}

	void SetLightPassTargets() const {
		DirectX::SimpleMath::Color clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderer_->GetContext().ClearRenderTargetView(accumulation_rtv_.Get(), clear_color);

		ID3D11RenderTargetView* views[] = {
			accumulation_rtv_.Get(),
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
		};
		renderer_->GetContext().OMSetRenderTargets(8, views, &renderer_->GetDepthStencilView());
	}

	void SetResources() {
		ID3D11ShaderResourceView* resources[] = {
			diffuse_srv_.Get(),
			normal_srv_.Get(),
			position_srv_.Get(),
		};
		renderer_->GetContext().PSSetShaderResources(0, 3, resources);
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
			static_cast<UINT>(renderer_->GetWindow().GetWidth()),
			static_cast<UINT>(renderer_->GetWindow().GetHeight()),
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
