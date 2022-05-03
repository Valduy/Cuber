#pragma once

#include <cassert>

#include "Renderer.h"

namespace graph {

class Sampler {
public:
	Sampler();

	HRESULT Init(
		Renderer* renderer,
		D3D11_TEXTURE_ADDRESS_MODE address_mode,
		D3D11_FILTER filter,
		D3D11_COMPARISON_FUNC comparison_func,
		UINT max_anisotropy);

	void SetSampler(unsigned int slot = 0);

private:
	Renderer* renderer_;
	D3D11_FILTER filter_;
	unsigned int max_anisotropy_;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_;
};

} // namespace graph