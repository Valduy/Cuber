#pragma once

#include "Renderer.h"

namespace graph {

class ConstantBuffer {
public:
	ConstantBuffer(size_t sizemem);

	HRESULT Init(Renderer* renderer);
	void SetBuffer();
	void Update(void* data);

private:
	Renderer* renderer_;
	size_t sizemem_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_;
};

} // namespace graph