#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Renderer.h"

namespace graph {

class IndexBuffer {
public:
	size_t GetSize();

	IndexBuffer(Renderer& renderer, std::vector<int> indices);
	HRESULT Init();
	void SetBuffer();

private:
	Renderer& renderer_;
	std::vector<int> indices_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer_;
};

} // namespace engine