#pragma once

#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl/client.h>

#include "Renderer.h"

namespace graph {

class Texture {
public:
	Texture()
		: renderer_(nullptr)
		, texture_(nullptr)
		, texture_view_(nullptr)
	{}

	HRESULT Init(Renderer* renderer, const DirectX::ScratchImage& image) {
		renderer_ = renderer;

		HRESULT result = CreateTextureEx(
			&renderer_->GetDevice(),
			image.GetImages(),
			image.GetImageCount(),
			image.GetMetadata(),
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
			0, 0, DirectX::CREATETEX_FLAGS::CREATETEX_DEFAULT,
			reinterpret_cast<ID3D11Resource**>(texture_.GetAddressOf()));

		if (FAILED(result)) {
			return result;
		}

		return renderer_->GetDevice().CreateShaderResourceView(texture_.Get(), nullptr, &texture_view_);
	}

	void SetTexture(int slot = 0) {
		assert(renderer_ != nullptr && "Texture isn't initialized.");
		renderer_->GetContext().PSSetShaderResources(slot, 1, texture_view_.GetAddressOf());
	}

private:
	Renderer* renderer_;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_view_;
};

} // namespace graph