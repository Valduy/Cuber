#pragma once

#include <d3d11.h>

#include "Renderer.h"

namespace graph {

class VertexBuffer {
public:
	VertexBuffer(void* vertexes, size_t sizemem);
	HRESULT Init(Renderer* renderer);
	void SetBuffer();

private:
	Renderer* renderer_;
	void* vertexes_;
	size_t sizemem_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
};

} // namespace engine