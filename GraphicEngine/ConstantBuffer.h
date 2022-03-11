#pragma once

#include "Renderer.h"

namespace graph {

class ConstantBuffer {
public:
	ConstantBuffer(Renderer& renderer, size_t size);

	HRESULT Init();
	void SetBuffer();
	void Update(void* data);

private:
	Renderer& renderer_;
	size_t size_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_;
};

} // namespace graph