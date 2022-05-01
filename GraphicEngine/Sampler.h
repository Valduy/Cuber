#pragma once

#include <cassert>

#include "Renderer.h"

namespace graph {

class Sampler {
public:
	Sampler(D3D11_FILTER filter, unsigned int max_anisotropy);

	HRESULT Init(Renderer* renderer);
	void SetSampler(unsigned int slot = 0);

private:
	Renderer* renderer_;
	D3D11_FILTER filter_;
	unsigned int max_anisotropy_;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state_;
};

} // namespace graph