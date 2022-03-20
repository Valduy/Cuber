#pragma once

#include <d3d11.h>

#include "Renderer.h"

namespace graph {

class VertexBuffer {
public:
	VertexBuffer(const void* vertexes, size_t sizemem);
	HRESULT Init(Renderer* renderer);
	void SetBuffer(unsigned int stribes);

private:
	Renderer* renderer_;
	const void* vertexes_;
	size_t sizemem_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
};

} // namespace engine