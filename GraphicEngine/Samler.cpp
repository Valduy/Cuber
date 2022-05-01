#include "pch.h"
#include "Sampler.h"

graph::Sampler::Sampler(D3D11_FILTER filter, unsigned int max_anisotropy)
	: renderer_(nullptr)
	, filter_(filter)
	, max_anisotropy_(max_anisotropy)
	, sampler_state_(nullptr)
{}

HRESULT graph::Sampler::Init(Renderer* renderer) {
	renderer_ = renderer;

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.Filter = filter_; // D3D11_FILTER_MIN_MAG_MIP_LINEAR
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_desc.BorderColor[0] = 1.0f;
	sampler_desc.BorderColor[1] = 0.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 1.0f;
	sampler_desc.MaxLOD = INT_MAX;
	sampler_desc.MaxAnisotropy = max_anisotropy_; // 0

	return renderer_->GetDevice().CreateSamplerState(&sampler_desc, &sampler_state_);
}

void graph::Sampler::SetSampler(unsigned int slot) {
	assert(renderer_ != nullptr && "Sampler isn't initialized.");
	renderer_->GetContext().PSSetSamplers(slot, 1, sampler_state_.GetAddressOf());
}
