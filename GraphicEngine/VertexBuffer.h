#pragma once

#include <vector>
#include <d3d11.h>
#include <SimpleMath.h>

#include "Renderer.h"

namespace graph {

class VertexBuffer {
public:
	VertexBuffer(Renderer& renderer, std::vector<DirectX::SimpleMath::Vector4> vertexes);
	HRESULT Init();
	void SetBuffer();

private:
	Renderer& renderer_;
	std::vector<DirectX::SimpleMath::Vector4> vertexes_;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
};

} // namespace engine