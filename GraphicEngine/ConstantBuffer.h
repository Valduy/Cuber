#pragma once

#include "Renderer.h"

namespace graph {

class ConstantBuffer {
public:
	ID3D11Buffer& GetBuffer();

	ConstantBuffer(size_t sizemem);

	HRESULT Init(Renderer* renderer);
	void SetBuffer(unsigned int slot = 0);
	void Update(const void* data) const;

private:
	Renderer* renderer_;
	size_t sizemem_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_;
};

} // namespace graph