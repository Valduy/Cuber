#pragma once

#include <wrl.h>
#include <d3d11.h>

#include "Window.h"

namespace graph {

	class Renderer {
	public:
		ID3D11Device& GetDevice() const;
		ID3D11DeviceContext& GetContext() const;

		Renderer(Window& window);
		HRESULT Init();
		void BeginRender();
		void EndRender();

	private:
		Window& window_;
		Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
		Microsoft::WRL::ComPtr<ID3D11Device> device_;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> back_texture_;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view_;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state_;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> raster_state_;		

		HRESULT CreateDeviceAndSwapChain();
		HRESULT GetBackTexture();
		HRESULT CreateDepthStencilBuffer();
		HRESULT CreateDepthStencilView();
		HRESULT CreateDepthStencilState();
		HRESULT CreateRenderTargetView();		
		HRESULT CreateRasterState();		
	};

} // namespace engine