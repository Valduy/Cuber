#include "pch.h"
#include "Sampler.h"

graph::Sampler::Sampler()
	: renderer_(nullptr)
	, sampler_state_(nullptr)
{}

HRESULT graph::Sampler::Init(
	Renderer* renderer,
	D3D11_TEXTURE_ADDRESS_MODE address_mode,
	D3D11_FILTER filter,
	D3D11_COMPARISON_FUNC comparison_func,
	UINT max_anisotropy)
{
	renderer_ = renderer;

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.AddressU = address_mode;
	sampler_desc.AddressV = address_mode;
	sampler_desc.AddressW = address_mode;
	sampler_desc.Filter = filter;
	sampler_desc.ComparisonFunc = comparison_func;
	sampler_desc.BorderColor[0] = 1.0f;
	sampler_desc.BorderColor[1] = 0.0f;
	sampler_desc.BorderColor[2] = 0.0f;
	sampler_desc.BorderColor[3] = 1.0f;
	sampler_desc.MaxLOD = INT_MAX;
	sampler_desc.MaxAnisotropy = max_anisotropy;

	return renderer_->GetDevice().CreateSamplerState(&sampler_desc, &sampler_state_);
}

void graph::Sampler::SetSampler(unsigned int slot) {
	assert(renderer_ != nullptr && "Sampler isn't initialized.");
	renderer_->GetContext().PSSetSamplers(slot, 1, sampler_state_.GetAddressOf());
}
