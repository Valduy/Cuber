#pragma once

#include "Renderer.h"

namespace graph {

class ShadowMap {
public:
	ShadowMap()
		: renderer_(nullptr)
		, shadow_map_(nullptr)
		, depth_buffer_(nullptr)
		, viewport_ {}
		, width_(0)
		, height_(0)
	{}

	HRESULT Init(Renderer* renderer, UINT width, UINT height) {
		renderer_ = renderer;
		width_ = width;
		height_ = height;
				
		CreateViewport();
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_map;
		HRESULT result;

		if (result = CreateDepthMap(depth_map.GetAddressOf()); FAILED(result)) {
			return result;
		}
		if (result = CreateDepthBuffer(depth_map.Get()); FAILED(result)) {
			return result;
		}
		if (result = CreateShadowMap(depth_map.Get()); FAILED(result)) {
			return result;
		}
		if (result = CreateRasterState(); FAILED(result)) {
			return result;
		}

		return result;
	}

	void SetRenderTarget() {
		assert(renderer_ != nullptr && "ShadowMap isn't initialized.");
		ID3D11RenderTargetView* render_targets[1] = { nullptr };
		renderer_->GetContext().RSSetState(raster_state_.Get());
		renderer_->GetContext().RSSetViewports(1, &viewport_);
		renderer_->GetContext().OMSetRenderTargets(1, render_targets, depth_buffer_.Get());
		renderer_->GetContext().ClearDepthStencilView(depth_buffer_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void SetTexture(int slot = 0) {
		assert(renderer_ != nullptr && "ShadowMap isn't initialized.");
		renderer_->GetContext().PSSetShaderResources(slot, 1, shadow_map_.GetAddressOf());
	}

private:
	Renderer* renderer_;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadow_map_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_buffer_;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState1> raster_state_;
	D3D11_VIEWPORT viewport_;
	UINT width_;
	UINT height_;

	void CreateViewport() {
		viewport_.TopLeftX = 0.0f;
		viewport_.TopLeftY = 0.0f;
		viewport_.Width = static_cast<float>(width_);
		viewport_.Height = static_cast<float>(height_);
		viewport_.MinDepth = 0.0f;
		viewport_.MaxDepth = 1.0f;
	}

	HRESULT CreateDepthMap(ID3D11Texture2D** depth_map) const {
		D3D11_TEXTURE2D_DESC depth_map_desc = {};
		depth_map_desc.Width = width_;
		depth_map_desc.Height = height_;
		depth_map_desc.MipLevels = 1;
		depth_map_desc.ArraySize = 1;
		depth_map_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depth_map_desc.SampleDesc.Count = 1;
		depth_map_desc.SampleDesc.Quality = 0;
		depth_map_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_map_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depth_map_desc.CPUAccessFlags = 0;
		depth_map_desc.MiscFlags = 0;

		return renderer_->GetDevice().CreateTexture2D(&depth_map_desc, nullptr, depth_map);
	}

	HRESULT CreateDepthBuffer(ID3D11Texture2D* depth_map) {
		D3D11_DEPTH_STENCIL_VIEW_DESC depth_buffer_desc = {};
		depth_buffer_desc.Flags = 0;
		depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_buffer_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_buffer_desc.Texture2D.MipSlice = 0;

		return renderer_->GetDevice().CreateDepthStencilView(
			depth_map, &depth_buffer_desc, depth_buffer_.GetAddressOf());
	}

	HRESULT CreateShadowMap(ID3D11Texture2D* depth_map) {
		D3D11_SHADER_RESOURCE_VIEW_DESC shadow_map_desc = {};
		shadow_map_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shadow_map_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shadow_map_desc.Texture2D.MipLevels = 1;
		shadow_map_desc.Texture2D.MostDetailedMip = 0;

		return renderer_->GetDevice().CreateShaderResourceView(
			depth_map, &shadow_map_desc, shadow_map_.GetAddressOf());
	}

	HRESULT CreateRasterState() {
		CD3D11_RASTERIZER_DESC1 raster_desc = {};
		raster_desc.CullMode = D3D11_CULL_BACK;
		raster_desc.FillMode = D3D11_FILL_SOLID;
		raster_desc.DepthBias = 10.0f;
		raster_desc.DepthBiasClamp = 0.0f;
		raster_desc.SlopeScaledDepthBias = 2.5f;

		Microsoft::WRL::ComPtr<ID3D11Device1> device_1_;
		HRESULT result = renderer_->GetDevice().QueryInterface(IID_ID3D11Device1, reinterpret_cast<void**>(device_1_.GetAddressOf()));

		if (FAILED(result)) {
			return result;
		}

		return device_1_->CreateRasterizerState1(&raster_desc, &raster_state_);
	}
};

} // namespace graph