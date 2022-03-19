#include "pch.h"
#include "Renderer.h"

#include <cassert>

ID3D11Device& graph::Renderer::GetDevice() {
	assert(device_ != nullptr && "Renderer isn't initialized.");
	return *device_.Get();
}

ID3D11DeviceContext& graph::Renderer::GetContext() {
	assert(context_ != nullptr && "Renderer isn't initialized.");
	return *context_.Get();
}

graph::Renderer::Renderer(Window& window)
	: window_(window)
	, swap_chain_(nullptr)
	, device_(nullptr)
	, context_(nullptr)
	, back_texture_(nullptr)
	, render_target_view_(nullptr)
	, raster_state_(nullptr) 
{}

HRESULT graph::Renderer::Init() {
	HRESULT result;
	window_.Show();

	if (result = CreateDeviceAndSwapChain(); FAILED(result)) {
		return result;
	}
	if (result = GetBackTexture(); FAILED(result)) {
		return result;
	}
	if (result = CreateDepthStencilBuffer(); FAILED(result)) {
		return result;
	}
	if (result = CreateDepthStencilView(); FAILED(result)) {
		return result;
	}
	if (result = CreateDepthStencilState(); FAILED(result)) {
		return result;
	}
	if (result = CreateRenderTargetView(); FAILED(result)) {
		return result;
	}
	if (result = CreateRasterState(); FAILED(result)) {
		return result;
	}

	return result;
}

void graph::Renderer::BeginRender() {
	context_->ClearState();	

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(window_.GetWidth());
	viewport.Height = static_cast<float>(window_.GetHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	constexpr float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context_->ClearRenderTargetView(render_target_view_.Get(), color);
	context_->ClearDepthStencilView(depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context_->RSSetState(raster_state_.Get());
	context_->OMSetDepthStencilState(depth_stencil_state_.Get(), 0);
	context_->RSSetViewports(1, &viewport);
	context_->OMSetRenderTargets(1, render_target_view_.GetAddressOf(), depth_stencil_view_.Get());
}

void graph::Renderer::EndRender() {
	swap_chain_->Present(1, 0);
}

HRESULT graph::Renderer::CreateDeviceAndSwapChain() {
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Width = window_.GetWidth();
	swap_chain_desc.BufferDesc.Height = window_.GetHeight();
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = window_.GetHandler();
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;

	D3D_FEATURE_LEVEL feature_level[] = { D3D_FEATURE_LEVEL_11_1 };

	return D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		feature_level,
		1,
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		&swap_chain_,
		&device_,
		nullptr,
		&context_);
}

HRESULT graph::Renderer::GetBackTexture() {
	ID3D11Texture2D* ptr = nullptr;
	HRESULT result = swap_chain_->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&ptr));

	if (FAILED(result)) {
		return result;
	}

	back_texture_ = ptr;
	return result;
}

HRESULT graph::Renderer::CreateDepthStencilBuffer() {
	D3D11_TEXTURE2D_DESC depth_stencil_desc;
	depth_stencil_desc.Width = window_.GetWidth();
	depth_stencil_desc.Height = window_.GetHeight();
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;

	return device_->CreateTexture2D(&depth_stencil_desc, nullptr, depth_stencil_buffer_.GetAddressOf());
}

HRESULT graph::Renderer::CreateDepthStencilView() {
	return device_->CreateDepthStencilView(depth_stencil_buffer_.Get(), nullptr, depth_stencil_view_.GetAddressOf());
}

HRESULT graph::Renderer::CreateDepthStencilState() {
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	return device_->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state_.GetAddressOf());
}

HRESULT graph::Renderer::CreateRenderTargetView() {
	return device_->CreateRenderTargetView(back_texture_.Get(), nullptr, &render_target_view_);
}

HRESULT graph::Renderer::CreateRasterState() {
	CD3D11_RASTERIZER_DESC raster_desc = {};
	raster_desc.CullMode = D3D11_CULL_NONE;
	raster_desc.FillMode = D3D11_FILL_SOLID;

	return device_->CreateRasterizerState(&raster_desc, &raster_state_);
}
