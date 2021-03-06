#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Renderer.h"

namespace graph {

class IndexBuffer {
public:
	size_t GetSize() const;

	IndexBuffer(const int* indices, size_t size);
	HRESULT Init(Renderer* renderer);
	void SetBuffer();

private:
	Renderer* renderer_;
	const int* indices_;
	size_t size_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer_;
};

} // namespace engine