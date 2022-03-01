#pragma once

#include <d3d11.h>
#include <directxmath.h>

#include "Renderer.h"

namespace graph {

	class VertexBuffer {
	public:
		VertexBuffer(Renderer& renderer, DirectX::XMFLOAT4* points, size_t size);
		HRESULT Init();
		void SetBuffer();

	private:
		Renderer& renderer_;
		DirectX::XMFLOAT4* points_;
		size_t size_;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
	};

} // namespace engine